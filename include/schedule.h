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

#define STACK_KB(name, size) static U8 __##name_top_stack[1024 * (size)]; \
U8* name = __##name_top_stack + sizeof((__##name_top_stack))

struct TaskParam_prv
{
    U32 pri;                //!< Priority level
};

typedef enum
{
    TASK_IDLE = 0,          //!< Initializing
    TASK_RUNNING,           //!< Active in scheduler
    TASK_LOCKED,            //!< Waiting for
} TaskStatus;

struct Task_prv
{
    U32 gpr[M4_GPR_N];  //!< General purpose register file
    U32 lr;    // r14
    U32 psr;            //!< Program status register
    U32 sp;    // r13
    U32 pc;    // r15

    U32 running;

#ifdef M4_FPU
    U32 spr[M4_SP_N];   //!< Single-precision registers
    U32 dpr[M4_DP_N];   //!< Double-precision registers
#endif

    Task* pri_next;         //!< Round robin pointer on same priority level
    Task* sched_next;
    TaskParam params;       //!< Task parameters
};

// Make sure the calculations in scheduler.S are correct
COMPILE_ASSERT(offsetof(Task, psr) == 56, psr_offset);
COMPILE_ASSERT(offsetof(Task, sp) == 60, sp_offset);
COMPILE_ASSERT(offsetof(Task, pc) == 64, pc_offset);
COMPILE_ASSERT(sizeof(PXX) == 4, pxx_size);

struct Scheduler_prv
{
    Task* task_rr_pri[OS_SCHED_PRI_N];
    Task* current_task;
    Task* sched_last;
};

void os_task_create(
        Task* self,
        const TaskParam* params,
        void* stack_ptr,
        void (*thread_main)(void*),
        void* arg);

Task* os_current_task(void);

#endif //TUMBARRTOS_SCHEDULE_H
