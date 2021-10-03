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
    .fpu softvfp
    .thumb

    .global __os_set_task_ctx

    .section  .text.__os_set_task_ctx
    # .weak  __os_set_task_ctx
    .type  __os_set_task_ctx, %function

__os_set_task_ctx:
    // Load all the registers except the SP, PC
    LDR r1, [r0, #56]   // Load psr
    ADD r0, r0, #8
    MSR APSR_nzcvqg, r1 // Write combined program status TODO(tumbar): Add mask
    LDM r0, {r2-r12,lr} // Load register file
    LDR sp, [r0, #60 - 8]   // Load sp

    // Load/push remaining temp registers target values
    LDR r1, [r0, #64 - 8]   // Grab pc
    PUSH {r1}

    LDR r1, [r0, #8]    // Grab r0
    PUSH {r1}

    LDR r1, [r0, #-4]   // Grab r1

    // Resume task execution
    POP {r0}
    POP {pc}
