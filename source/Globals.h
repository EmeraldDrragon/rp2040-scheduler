#ifndef MICROCONTROLLER_UE_GLOBALS_H
#define MICROCONTROLLER_UE_GLOBALS_H

#include "Task.h"
#include "hardware/sync.h"

#define SWITCH_SIGNAL 0xFF
#define TASK_SPINLOCK_ID PICO_SPINLOCK_ID_OS1

extern volatile Task* current_task;
extern volatile Task* next_task;
extern volatile bool first_task_started;
extern volatile bool core_one_ready;
extern spin_lock_t* task_spinlock;

#endif //MICROCONTROLLER_UE_GLOBALS_H