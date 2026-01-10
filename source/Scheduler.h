#ifndef MICROCONTROLLER_UE_SCHEDULER_H
#define MICROCONTROLLER_UE_SCHEDULER_H

#include <vector>

#include "Task.h"

class Scheduler
{
public:
    std::vector<Task*> task_list;
    Scheduler();
    void addTask(Task* t)
    {
        task_list.push_back(t);
    }
    void start_scheduler();
    void scheduler_logic();
};


#endif //MICROCONTROLLER_UE_SCHEDULER_H