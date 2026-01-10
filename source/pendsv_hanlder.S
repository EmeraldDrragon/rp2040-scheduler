.cpu cortex-m0plus
.thumb
.syntax unified

@ ___ define globals ___

.equ SIO_BASE, 0xd0000000
.equ SPINLOCK_ID, 14
.equ SPINLOCK0_OFFSET, 0x100
.equ SPINLOCK_ADDR, (SIO_BASE + SPINLOCK0_OFFSET + (SPINLOCK_ID * 4))

.extern current_task
.extern next_task
.extern first_task_started

.global pendsv_handler
.type pendsv_handler, %function

.section .text

@ ___ pendsv handler ___
pendsv_handler:

@ ___ disable interrupts ___
    CPSID i

@ ___ check first task started bool ___
    LDR r1, =first_task_started
    LDRB r2, [r1]
    CMP r2, #0

@ ___ if false skip saving ___
    BEQ skip_save

@ ___ save software context ___
    MRS r0, psp
    SUBS r0, r0, #32
    STR r4, [r0, #0]
    STR r5, [r0, #4]
    STR r6, [r0, #8]
    STR r7, [r0, #12]

    MOV r4, r8
    MOV r5, r9
    MOV r6, r10
    MOV r7, r11
    STR r4, [r0, #16]
    STR r5, [r0, #20]
    STR r6, [r0, #24]
    STR r7, [r0, #28]

@ ___ update current_task->sp ___
    LDR r1, =current_task
    LDR r1, [r1]
    STR r0, [r1]

skip_save:

@ ___ acquire lock ___
    LDR r1, =SPINLOCK_ADDR
lock_loop:
    LDR r2, [r1]
    CMP r2, #0
    BEQ lock_loop

@ ___ get new task and update current task ___
    LDR r1, =next_task
    LDR r2, [r1]
    LDR r1, =current_task
    STR r2, [r1]

@ ___ release lock ___
    LDR r1, =SPINLOCK_ADDR
    MOVS r0, #1
    STR r0, [r1]

@ ___ update first task started flag ___
    LDR r1, =first_task_started
    MOVS r0, #1
    STRB r0, [r1]

@ ___ load software context ___
    LDR r1, =current_task
    LDR r1, [r1]
    LDR r0, [r1]
    LDR r4, [r0, #16]
    LDR r5, [r0, #20]
    LDR r6, [r0, #24]
    LDR r7, [r0, #28]
    MOV r8, r4
    MOV r9, r5
    MOV r10, r6
    MOV r11, r7
    LDR r4, [r0, #0]
    LDR r5, [r0, #4]
    LDR r6, [r0, #8]
    LDR r7, [r0, #12]
    ADDS r0, r0, #32
    MSR psp, r0

@ ___ enable interrupts ___
    CPSIE i

@ ___ return from handler ___
    BX LR

