/*
 * This file is part of the TumbarRTOS
 * Copyright (c) 2021 Andrei Tumbar.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

    .syntax unified
    .cpu cortex-m4
    .thumb

    #include <arm.h>

ARM_ASM_FUNC(__os_set_task_ctx):
    // Load all the registers except the SP, PC
    ldr r1, [r0, #56]       // Load psr
    add r0, r0, #8
    msr APSR_nzcvqg, r1     // Write combined program status
    ldm r0, {r2-r12,lr}     // Load register file
    ldr sp, [r0, #60 - 8]   // Load sp

    // Load/push remaining temp registers target values
    ldr r1, [r0, #64 - 8]   // Grab pc
    push {r1}

    ldr r1, [r0, #8]        // Grab r0
    push {r1}

    ldr r1, [r0, #-4]       // Grab r1

    // Resume task execution
    pop {r0}
    cpsie I                 // re-enable interrupts
    pop {pc}

ARM_ASM_FUNC(SysTick_Handler):
    cpsid I                 // Disable interrupts
    push {lr}
    bl tim_isr              // Increment system time
    bl HAL_IncTick          // Increment HAL system time
    ldr r1, =os_running
    ldr r0, [r1]
    tst r0, r0              // Check if the ctx switch is already running
    beq __ctx_switch_arm
    cpsie I                 // Enable interrupts
    pop {pc}                // Exit the handler

__ctx_switch_arm:
    pop {lr}
    // Save the context
    // Stack has the following:
    // xPSR   <----- sp
    // lr
    // r12
    // r0 - r3
    // pc
    push {r4-r11}

    // Point the first argument to the stack
    mov r0, sp
    push {lr}
    bl os_task_isr

    // The above function exited, meaning a context change should not happen
    pop {lr}
    pop {r4-r11}
    bx lr

ARM_ASM_FUNC(__os_relinquish):
    // We don't need to store any low register values
    // This is not an interrupt so that context save is done
    // by the compiler. We just need to store sp, lr, pc
//    add r0, #4      // We don't need to store r0, it's the return value
//    stmia r0!, {r1, r2, r3, r4, r5, r6, r7}
//    str r8, [r0, #0]
//    str r9, [r0, #4]
//    str r10, [r0, #8]
//    str r11, [r0, #12]
//    str r12, [r0, #16]
//    str r13, [r0, #20]
    str lr, [r0, #52]
    str sp, [r0, #60]
    str lr, [r0, #64]   // store lr in pc (we want to continue execution with this task)

    // Schedule another task
    b os_sched_next
