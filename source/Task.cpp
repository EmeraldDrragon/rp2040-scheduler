#include "Task.h"
#include "pico/multicore.h"



Task::Task(void (*task_routine)())
{
    //xPSR
    stack[STACK_SIZE - 1] = 0x01000000;
    //PC
    stack[STACK_SIZE - 2] = reinterpret_cast<uint32_t>(task_routine) | 1;
    //LR
    stack[STACK_SIZE - 3] = reinterpret_cast<uint32_t>(&Task::zombieTask) | 1;
    //sp pointing to the end of array
    //after space reserved for saving registers
    sp = &stack[STACK_SIZE-16];
}

void Task::zombieTask()
{
    while(1)
    {
        __wfi();
    }
}