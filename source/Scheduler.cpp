#include "Scheduler.h"
#include "Globals.h"
#include "pico/time.h"
#include "pico/multicore.h"
#include "hardware/sync.h"


void throwAwayTaskRoutine()
{
    while(1);
}

Task throwaway_task(throwAwayTaskRoutine);

Scheduler::Scheduler()
{
    addTask(&throwaway_task);
}

void Scheduler::start_scheduler()
{
    if(task_list.empty()) return;

    current_task = task_list.at(0);
    next_task = current_task;

    first_task_started = false;

    //wait for core 1 to be ready
    while(!core_one_ready)
    {}

    scheduler_logic();
}

void Scheduler::scheduler_logic()
{
    size_t current_index = 0;
    bool cleaned = false;

    while(1)
    {
        //time slice for debugging
        sleep_ms(3000);

        current_index = (current_index + 1) % task_list.size();

        if(!cleaned && first_task_started)
        {
            uint32_t saved_irq = spin_lock_blocking(task_spinlock);
            task_list.erase(task_list.begin());
            current_index = 0;
            cleaned = true;
            spin_unlock(task_spinlock, saved_irq);
        }
        uint32_t saved_irq = spin_lock_blocking(task_spinlock);
        next_task = task_list.at(current_index);
        spin_unlock(task_spinlock, saved_irq);

        multicore_fifo_push_blocking(SWITCH_SIGNAL);
    }
}