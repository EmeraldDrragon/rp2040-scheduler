# Dual-Core Preemptive RTOS for RP2040

Real-time mini operating system for multitasking
built from scratch for Raspberry Pi PICO RP2040 (Cortex-M0+),
featuring multicore IRQ's, assembly-level context switching, 
stack memory manipulation, and hardware-synchronized scheduling.

## Design

 * **Dual-Core** - Developed utilizing dual-core architecture of RP2040. The system
is divided into kernel logic on core 0 and worker logic on core 1.

 * **Task Scheduling** - The kernel schedules tasks via Round-Robin strategy,
with a fixed time slices (for demonstration and debugging purposes it is set to 3000ms,
to see the task transition on shared hardware resources (same GPIO))

 * **Tasks** - To add a task to the scheduler, the user needs to create
a routine function, initialize the task as a global object with this routine, passed
as an argument to constructor and add this task to the scheduler's task list.

## Key Implementations

### NVIC and Vector Table Relocation
The Vector Table was relocated from Flash to SRAM, to be able to read and write
custom IRQ handlers and set their priority. PendSV handler was assigned the lowest priority to ensure that
context switches only occur after all hardware IRQs are done, while
the SIO interrupt was assigned the highest priority for immediate preemption triggers.

### Dual Core interrupt requests
Scheduling signals are passed across cores using the RP2040's hardware FIFO.
Core 1's SIO FIFO handler catches the signal that any data was passed ove FIFO from core 0,
clears the FIFO, and manually sets the PendSV bit in the ICSR to pend the context switch.

### Scheduling
The Scheduler passes data over FIFO (can be anything, defined 0xFF as SWITCH_SIGNAL) after setting
global variable next_task, using a round-robin strategy.

### Thread Mode and Stack Pointers
The critical feature of the processor that was used is the separation of execution modes and stack pointers.
When the processor is executing normal code it runs in the thread mode, when it executes exception handler
it switches to handler mode. MSP (Main Stack Pointer) is used by the CPU during boot and inside Exception handlers.
PSP (Process Stack Pointer) is used by application tasks in the multitasking system.
When the core one starts, after relocating the vector table and setting up the priority for interrupts, it
manually sets the address for PSP and enables the use of PSP by modifying the CONTROL special register.

### Tasks and stacks
Task stacks are 8-byte aligned arrays inside global objects. The class Task also contains a stack pointer, that has
the current position of a PSP that needs to be saved and restored during the context switch. Tasks objects are globally instantiated
to ensure persistence, preventing collisions, as well as 0-initialization.

### Context Switch
Context Switch is done in the pendsv_handler, written in ARM assembly. Cortex-M0+ automatically saves
R0-R3, R12, LR, PC, xPSR registers, the handler manually saves and restores R4-R11 registers.

## Challenges
 * **First Task corruption bug** During the initial transition from the idle state to the first user task, the hardware automatic stacking
would overwrite the entry point of the first task in the list. solved by creating a bootstrap task to be deleted after the first context switch, 
since only the first task in the list was getting corrupted, ensuring that all user tasks are not getting corrupted.

 * **Memory Map and Stack Location** Originally planned for stacks to be one after another in the SRAM region, in the uninitialized
memory space between Scratch X and Heap, and later decided to make stacks as global objects, for easier stack borders initialization.

 * **PendSV priority setup** It wasnt possible for me to find the specific high level function in the PICO SDK to set the priority for
the PendSV exception, so instead I implemented direct register access to the SHPR3 and manually set the priority to 3 (the lowest possible).

## Future Plans
 * Priority-Based scheduling
 * Mutexes and Semaphores

## References
 * The Definitive Guide to ARM Cortex-M0 and Cortex-M0+ Processors
 * RP2040 Datasheet A microcontroller by Raspberry Pi
 * Raspberry Pi Documentation Pico C SDK

