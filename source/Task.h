#ifndef MICROCONTROLLER_UE_TASK_H
#define MICROCONTROLLER_UE_TASK_H
#include <cstdint>

#define STACK_SIZE 256

class Task
{
public:
    uint32_t* sp;
    alignas(8) uint32_t stack[STACK_SIZE];

    Task(void (*task_routine)());

    static void zombieTask();
};


#endif //MICROCONTROLLER_UE_TASK_H