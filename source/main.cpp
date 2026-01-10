#include "task.h"
#include "Scheduler.h"
#include "CoreOne.h"
#include "pico/time.h"
#include "pico/stdio.h"
#include "pico/multicore.h"
#include "pico/stdio.h"
#include "stdio.h"
#include "hardware/gpio.h"

//Controls:
// 1. Create routine functions for tasks
// 2. Create global task objects
// 3. create scheduler instance
// 4. add tasks to the scheduler
// 5. Start core 1 with coreOne logic
// 6. start the scheduler
// note: the scheduler does not return,
// except when called without added tasks

void busy_wait_delay(uint32_t ms) {
    for(volatile uint32_t i = 0; i < ms * 10000; i++) {
        __asm("nop");
    }
}

void taskRoutineA()
{
    while(1)
    {
        gpio_put(25, 1);
        busy_wait_delay(250);
        gpio_put(25, 0);
        busy_wait_delay(250);
    }
}
void taskRoutineB()
{
    while(1)
    {
        gpio_put(25, 1);
        busy_wait_delay(500);
        gpio_put(25, 0);
        busy_wait_delay(500);
    }
}
void taskRoutineC()
{
    while(1)
    {
        gpio_put(25, 1);
        busy_wait_delay(50);
        gpio_put(25, 0);
        busy_wait_delay(50);
    }
}

Task task_a(taskRoutineA);
Task task_b(taskRoutineB);
Task task_c(taskRoutineC);

int main()
{
    stdio_init_all();
    sleep_ms(2000);
    printf("starting os\n");
    gpio_init(25);
    gpio_set_dir(25,GPIO_OUT);

    Scheduler scheduler_instance;
    scheduler_instance.addTask(&task_a);
    scheduler_instance.addTask(&task_b);
    scheduler_instance.addTask(&task_c);

    multicore_launch_core1(CoreOne::coreOneLogic);

    scheduler_instance.start_scheduler();
}
