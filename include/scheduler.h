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


#ifndef TUMBARRTOS_SCHEDULER_H
#define TUMBARRTOS_SCHEDULER_H

#include <global.h>
#include <rtos.h>

typedef struct TaskContext_prv TaskContext;
typedef struct Scheduler_prv Scheduler;
typedef struct Event_prv Event;

#define STACK_WORDS(name, num_words) static U8 __##name_top_stack[sizeof(PXX) * (num_words)]; \
U8* name = __##name_top_stack + sizeof((__##name_top_stack))

struct TaskParam_prv
{
    U32 pri;                //!< Priority level
};

typedef enum
{
    TASK_BLOCKED = 0,       //!< Waiting for event
    TASK_READY,             //!< Active in scheduler
    TASK_SUSPEND,           //!< Waiting to be manually reactivated
    TASK_STOPPED            //!< Execution has finished
} TaskStatus;

struct TaskContext_prv
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
};

// Make sure the calculations in scheduler.S are correct
COMPILE_ASSERT(offsetof(TaskContext, lr) == 52, lr_offset);
COMPILE_ASSERT(offsetof(TaskContext, psr) == 56, psr_offset);
COMPILE_ASSERT(offsetof(TaskContext, sp) == 60, sp_offset);
COMPILE_ASSERT(offsetof(TaskContext, pc) == 64, pc_offset);
COMPILE_ASSERT(sizeof(PXX) == 4, pxx_size);

struct Task_prv
{
    TaskContext context;    //!< CPU context
    TaskParam params;       //!< Task parameters

    TaskStatus state;       //!< Current state
    U32 last_service;       //!< Last timestamp when this task was executed

    Event* blocker;         //!< Event we are waiting on (NULL if not BLOCKED)

    Task* next;             //!< Creates linked list of tasks
    Task* prev;             //!< Previous in linked list
};

struct Event_prv
{
    U16 event_idx;                  //!< Index in the event table
    U16 event_mask;                 //!< Bitmask for the event

    /**
     * Event handler is run when the event fires
     * It is able to change the state of its blocking task
     */
    void (*event_handler)(Task*);
};


COMPILE_ASSERT((OS_MAX_EVENT % 2) == 0, os_event_multiple_of_2);
struct Scheduler_prv
{
    Task* task_rr_pri[OS_SCHED_PRI_N];      //!< Task table
    Task* current_task;                     //!< Currently executing task

    U16 event_table[OS_MAX_EVENT];          //!< Bit table of events
    U32 event_registered;                   //!< Number of events registered
};

extern volatile I32 os_running;

#define OS_CRITICAL_ENTER() os_running = 1
#define OS_CRITICAL_EXIT() os_running = 0
#define OS_CRITICAL(expr) do { OS_CRITICAL_ENTER(); { expr; } OS_CRITICAL_EXIT(); } while (0)

/**
 * Ask the os for a new event bit in the
 * scheduler event table
 * @param self Event to initialize event to
 * @return 0 for success, non-zero for error
 */
I32 os_sched_alloc_event(Event* self);

/**
 * Block the current task to wait for an event to fire
 * @param self Event that caused os relinquish
 */
void os_sched_block(Event* self);

/**
 * Fire an event on the scheduler to trigger check on blocking tasks
 * @param self Event with event to fire
 */
void os_sched_event_fire(Event* self);

/**
 * Clear an event
 * @param self Event to clear
 */
void os_sched_event_clear(Event* self);

void os_scheduler_main(void);

#endif //TUMBARRTOS_SCHEDULER_H
