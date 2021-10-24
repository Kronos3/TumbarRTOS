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
    TASK_READY,             //!< Active in scheduler
    TASK_BLOCKED,           //!< Waiting for event
    TASK_SUSPEND,           //!< Waiting to be manually reactivated
    TASK_STOPPED            //!< Execution has finished
} TaskStatus;

struct Task_prv
{
    PXX* sp;                //!< Last stack pointer of task (psp)

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

    U16 event_table[OS_MAX_EVENT];          //!< Bit table of events
    U32 event_registered;                   //!< Number of events registered
};

#define OS_DISABLE_INTERRUPTS() __asm volatile (" cpsid i")
#define OS_ENABLE_INTERRUPTS() __asm volatile (" cpsie i")

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

void os_task_initialize_stack(Task* self,
                              void (*entry) (void*),
                              void* argument);

void os_pend_sv(void);

extern void os_start_first_task(void);
extern void xPortPendSVHandler(void);

#endif //TUMBARRTOS_SCHEDULER_H
