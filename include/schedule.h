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


#ifndef TUMBARRTOS_SCHEDULE_H
#define TUMBARRTOS_SCHEDULE_H

#include <stddef.h>
#include <global.h>

#define OS_SCHED_PRI_N 32

// General purpose registers (excluding SP, LR, PC)
#define M4_GPR_N 13

// Single precision registers
#define M4_SP_N 32

// Double precision registers
#define M4_DP_N 16



typedef struct Task_prv Task;
typedef struct TaskParam_prv TaskParam;
typedef struct Scheduler_prv Scheduler;

struct TaskParam_prv
{
    U32 pri;                //!< Priority level
};

struct Task_prv
{
    U32 gpr[M4_GPR_N];  //!< General purpose register file
    U32 lr;    // r14
    U32 psr;            //!< Program status register
    U32 sp;    // r13
    U32 pc;    // r15

#ifdef M4_FPU
    U32 spr[M4_SP_N];   //!< Single-precision registers
    U32 dpr[M4_DP_N];   //!< Double-precision registers
#endif

    Task* sched_next;       //!< Round robin pointer on same priority level
    TaskParam params;       //!< Task parameters
};

// Make sure the calculations in scheduler.s are correct
COMPILE_ASSERT(offsetof(Task, psr) == 56, psr_offset);
COMPILE_ASSERT(offsetof(Task, sp) == 60, sp_offset);
COMPILE_ASSERT(offsetof(Task, pc) == 64, pc_offset);
COMPILE_ASSERT(sizeof(PXX) == 4, pxx_size);

struct Scheduler_prv
{
    Task* task_rr_pri[OS_SCHED_PRI_N];
};

void os_task_create(
        Task* self,
        const TaskParam* params,
        void* stack_ptr,
        void (*thread_main)(void*),
        void* arg);

#endif //TUMBARRTOS_SCHEDULE_H
