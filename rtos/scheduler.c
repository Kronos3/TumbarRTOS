#include <sys/cdefs.h>
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

#include <rtos.h>
#include <scheduler.h>
#include <tim.h>

#include <stm32l4xx.h>

void* memset(void* s, int c, int n);

// The idle task will simply absorb cycles when no
// task is ready to run
_Noreturn static void idle_task_main(void);

static const TaskParam default_params = {
        .pri = 15, // medium priority
};

static U32 idle_stack[20];
static Task idle_task;

static Scheduler os_scheduler = {
        .task_rr_pri = {NULL},
        .event_table = {0},
        .event_registered = 0,
};

Task* volatile os_current_tcb;
Task* volatile os_next_tcb;

static bool_t os_started = FALSE;
static const Task volatile* current_task;

__attribute__ ((noreturn))
static void idle_task_main(void)
{
    while (1);
}

/**
 * Private function that will execute after
 * the execution of a thread has finished
 */
static void os_task_exit(void)
{
    // Remove this task from execution
    Task* self = os_current_task();
    if (self->next)
    {
        self->next->prev = self->prev;
    }

    if (self->prev)
    {
        self->prev->next = self->next;
    }

    if (os_scheduler.task_rr_pri[self->params.pri] == self)
    {
        os_scheduler.task_rr_pri[self->params.pri]->next = self->next;
    }

    self->state = TASK_STOPPED;
    // TODO(tumbar) pendSV
}

void os_task_create(
        Task* self,
        const TaskParam* params,
        void* stack_ptr,
        void (* thread_main)(void*),
        void* arg)
{
    FW_ASSERT(self);
    FW_ASSERT(stack_ptr);
    FW_ASSERT(thread_main);


    if (!params)
    {
        params = &default_params;
    }

    // Store provided parameters
    FW_ASSERT(params->pri < OS_SCHED_PRI_N, params->pri);
    self->sp = stack_ptr;
    self->params = *params;
    self->state = TASK_READY;
    self->blocker = NULL;
    self->last_service = 0; // never been serviced

    os_task_initialize_stack(self, thread_main, arg);

    // Add to linked list
    self->next = os_scheduler.task_rr_pri[params->pri];
    self->prev = NULL;

    if (self->next)
    {
        self->next->prev = self;
    }

    os_scheduler.task_rr_pri[params->pri] = self;
}

bool_t os_sched_next(void)
{
    Scheduler* self = &os_scheduler;

    // Tell the task about when it was serviced
    U32 curr_tim = tim_get();

    // Current TCB will not be set on first
    // task scheduling, in this case no task
    // has been serviced
    if (os_current_tcb)
    {
        os_current_tcb->last_service = curr_tim;
    }

    // Update all the task states
    for (U32 i = 0; i < OS_SCHED_PRI_N; i++)
    {
        for (Task* pri_curr = self->task_rr_pri[i]; pri_curr; pri_curr = pri_curr->next)
        {
            switch (pri_curr->state)
            {
                case TASK_BLOCKED:
                    // Service the blocker to check if it's now ready to run
                    // Check the event
                    FW_ASSERT(pri_curr->blocker && "Task blocked without blocker");
                    FW_ASSERT(pri_curr->blocker->event_handler && "Event has no handler");
                    if (self->event_table[pri_curr->blocker->event_idx] & pri_curr->blocker->event_mask)
                    {
                        // Event has fired
                        // Run the isr
                        pri_curr->blocker->event_handler(pri_curr);
                    }
                    break;
                case TASK_READY:
                case TASK_SUSPEND:
                    // Don't do anything
                    break;
                default:
                    FW_ASSERT(0 && "Invalid task state", pri_curr->state);
            }
        }
    }

    // Find the next task that should execute
    Task* next_task = NULL;
    U32 tim_delta = 0;

    for (U32 i = 0; i < OS_SCHED_PRI_N && !next_task; i++)
    {
        for (Task* pri_curr = self->task_rr_pri[i]; pri_curr; pri_curr = pri_curr->next)
        {
            if (pri_curr->state == TASK_READY)
            {
                // Check if we have chosen a task yet
                // Check if this task needs servicing more than the last chosen task
                if (!next_task || curr_tim - pri_curr->last_service > tim_delta)
                {
                    // This task is older than the previously chosen task
                    // Implements round-robin (oldest first)
                    next_task = pri_curr;
                    tim_delta = curr_tim - pri_curr->last_service;
                }
            }
        }
    }

    // If no tasks are ready to execute, run the idle task
    if (!next_task) next_task = &idle_task;

    os_next_tcb = next_task;
    return os_next_tcb != os_current_tcb;
}

Task* os_current_task(void)
{
    return os_current_tcb;
}

void os_sched_block(Event* self)
{
    Task* task = os_current_task();
    task->blocker = self;
    task->state = TASK_BLOCKED;
}

I32 os_sched_alloc_event(Event* self)
{
    if (os_scheduler.event_registered >= sizeof(os_scheduler.event_table) * 8)
    {
        // Error, failed to allocate event
        return 1;
    }

    self->event_idx = os_scheduler.event_registered >> 4;
    self->event_mask = 0x1 << (os_scheduler.event_registered & 0x0F);
    return 0;
}

void os_sched_event_fire(Event* self)
{
    os_scheduler.event_table[self->event_idx] |= self->event_mask;
}

void os_sched_event_clear(Event* self)
{
    os_scheduler.event_table[self->event_idx] &= ~(self->event_mask);
}

void os_scheduler_main(void)
{
    OS_DISABLE_INTERRUPTS();

    // Initialize the idle task
    memset(&idle_task, 0, sizeof(idle_task));
    idle_task.sp = idle_stack + sizeof(idle_stack);
    idle_task.next = os_scheduler.task_rr_pri[OS_SCHED_PRI_N - 1];
    if (os_scheduler.task_rr_pri[OS_SCHED_PRI_N - 1])
    {
        os_scheduler.task_rr_pri[OS_SCHED_PRI_N - 1]->prev = &idle_task;
    }

    os_scheduler.task_rr_pri[OS_SCHED_PRI_N - 1] = &idle_task;

    os_sched_next();
    os_task_initialize_stack(
            os_next_tcb,
            (void (*)(void*)) idle_task_main,
            NULL);
    os_started = TRUE;
    os_current_tcb = os_next_tcb;
    os_start_first_task();
    FW_ASSERT(0 && "Start task didn't work");
}

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR                      ( 0x01000000 )
#define portINITIAL_EXC_RETURN                ( 0xfffffffd )

void os_task_initialize_stack(
        Task* self,
        void (*entry) (void*),
        void* argument)
{
    *--self->sp = portINITIAL_XPSR; // xPSR
    *--self->sp = (PXX) entry; // pc
    *--self->sp = (PXX) os_task_exit; // lr
    self->sp -= 4; // r12, r13, r2, r1
    *--self->sp = (PXX) argument; // r0
    *--self->sp = portINITIAL_EXC_RETURN;
    self->sp -= 8; // r11 - r4
}

void os_task_context_switch(void)
{
    OS_DISABLE_INTERRUPTS();
    {
        FW_ASSERT(os_next_tcb);
        os_current_tcb = os_next_tcb;
        os_next_tcb = NULL;
    }
    OS_ENABLE_INTERRUPTS();
}

void os_pend_sv(void)
{
    // Pend a context switch service
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void xPortSysTickHandler(void)
{
    // Interrupting in the middle of SysTick will
    // cause issues with the rest of the OS
    OS_DISABLE_INTERRUPTS();
    if (os_started)
    {
        // Increment the system time
        tim_isr();

        // Check is we need to perform a context switch
        if (os_sched_next())
        {
            os_pend_sv();
        }
        else
        {
            // No context switch is needed
            // Clear next TCB
            os_next_tcb = NULL;
        }
    }
    OS_ENABLE_INTERRUPTS();
}
