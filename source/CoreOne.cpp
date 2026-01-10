#include "CoreOne.h"
#include "stdio.h"
#include "pico/time.h"
#include "pico/stdio.h"
#include "pico/multicore.h"
#include "hardware/structs/scb.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/regs/m0plus.h"
#include "Task.h"
#include "Globals.h"

#define LED_PIN 25

volatile Task* current_task = nullptr;
volatile Task* next_task = nullptr;
volatile bool first_task_started = false;
volatile bool core_one_ready = false;
spin_lock_t* task_spinlock = spin_lock_init(TASK_SPINLOCK_ID);

volatile int led_on = 0;

static uint32_t ram_vector_table[48] __attribute__((aligned(256)));

extern "C" void pendsv_handler();

void __isr isr_sio_proc1()
{
    while(multicore_fifo_rvalid())
    {
        multicore_fifo_pop_blocking();
    }
    scb_hw->icsr = M0PLUS_ICSR_PENDSVSET_BITS;
    multicore_fifo_clear_irq();
}

void CoreOne::coreOneLogic()
{
    //relocate interrupt table
    uint32_t *flash_vtable = (uint32_t*)scb_hw->vtor;
    for(int i = 0; i < 48; i++)
    {
        ram_vector_table[i] = flash_vtable[i];
    }
    scb_hw->vtor = (uint32_t)ram_vector_table;
    ram_vector_table[14] = (uint32_t)pendsv_handler;
    irq_set_exclusive_handler(SIO_IRQ_PROC1, isr_sio_proc1);

    //set priority for sio irq proc
    irq_set_enabled(SIO_IRQ_PROC1, true);
    irq_set_priority(SIO_IRQ_PROC1, 0);

    //set priority for pendsv
    volatile uint32_t* shpr3_reg = (volatile uint32_t*)(PPB_BASE + M0PLUS_SHPR3_OFFSET);
    uint32_t reg_val = *shpr3_reg;
    reg_val &= ~M0PLUS_SHPR3_PRI_14_BITS;
    reg_val |= (3u << M0PLUS_SHPR3_PRI_14_LSB);
    *shpr3_reg = reg_val;

    //wait for current_task to be setup and
    //set up the psp
    while(current_task == nullptr)
    {}
    __asm volatile("msr psp, %0"
        :
        : "r" (current_task->sp)
        );

    uint32_t control_r;
    __asm volatile("mrs %0, control"
        : "=r"(control_r)
        );
    control_r |= 2;
    __asm volatile("msr control, %0"
        :
        : "r" (control_r)
        );
    __asm volatile("isb");

    core_one_ready = true;

    //wait for interrupt to switch to psp execution
    while(1)
    {
        __wfi();
    }
}
