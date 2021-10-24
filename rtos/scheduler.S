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
    #include <RTOSConfig.h>

ARM_ASM_FUNC(xPortPendSVHandler):
    mrs r0, psp
    isb

    ldr r3, os_current_tcb_const // load the current tcb
    ldr r2, [r3]

    tst r14, #0x10              // check if the FPU is in use
    it eq
    vstmdbeq r0!, {s16-s31}     // save the FPU registers

    stmdb r0!, {r4-r11, r14}    // save core registers
    str r0, [r2]                // save the current stack position into the tcb

    push {r0, r3}
    mov r0, configMAX_SYSCALL_INTERRUPT_PRIORITY
    msr basepri, r0
    dsb
    isb

    bl os_task_context_switch
    mov r0, #0
    msr basepri, r0
    pop {r0, r3}

    // Load the stack of the next task
    ldr r1, [r3]
    ldr r0, [r1]

    ldmia r0!, {r4-r11, r14}    // restore the core registers

    tst r14, #0x10              // check if the fpu was in use
    it eq
    vldmiaeq r0!, {s16-s31}     // restore fpu registers

    msr psp, r0
    isb

    bx r14
    .align 4

os_current_tcb_const: .word os_current_tcb
os_next_tcb_const: .word os_next_tcb

ARM_ASM_FUNC(SVC_Handler):
    ldr r3, os_current_tcb_const
    ldr r2, [r3]
    ldr r0, [r2]    // load the sp
    ldmia r0!, {r4-r11, r14}    // restore core registers
    msr psp, r0
    isb
    mov r0, #0
    msr basepri, r0
    bx r14


ARM_ASM_FUNC(os_start_first_task):
    ldr r0, =0xE000ED08     // Use the NVIC offset register to locate the system stack
    ldr r0, [r0]
    ldr r0, [r0]
    msr msp, r0             // Reset stack position to original point
    mov r0, #0
    msr control, r0         // clear control
    cpsie i                 // enable interrupts (CPU)
    cpsie f                 // enable interrupts (FPU)
    dsb
    isb
    svc 0                   // initiate first task
    nop
    bx lr                   // should not be reached
    .ltorg
