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

void* memset(void* s, int c, int n);
extern __attribute__((noreturn)) void __os_set_task_ctx(TaskContext* self);
U32 __os_relinquish(TaskContext* self);
void os_sched_next(void);

// The idle task will simply absorb cycles when no
// task is ready to run
static void idle_task_exit(void);
static void idle_task_main(void);

static I32 scheduler_running = 0;
volatile I32 os_running = 0;

static const TaskParam default_params = {
        .pri = 15, // medium priority
};

static U32 idle_stack[2];
static Task idle_task = {
        .context = {
                .gpr = {0},
                .lr = (PXX)idle_task_exit,
                .pc = (PXX)idle_task_main,
                .psr = 0x0,
                .sp = (PXX)(idle_stack + 2)
        }
};

static Scheduler os_scheduler = {
        .task_rr_pri = {NULL},
        .current_task = NULL,
        .event_table = {0},
        .event_registered = 0
};

static const Task volatile* current_task;

static void idle_task_exit(void)
{
    FW_ASSERT(0 && "Idle task has exited");
}

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
    OS_CRITICAL_ENTER();
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
    os_sched_next();
}

void os_task_create(
        Task* self,
        const TaskParam* params,
        void* stack_ptr,
        void (* thread_main)(void*),
        void* arg)
{
    // Clear the register files
    memset(self->context.gpr, 0, sizeof(self->context.gpr));

#ifdef M4_FPU
    memset(self->gpr, 0, sizeof(self->spr));
    memset(self->gpr, 0, sizeof(self->dpr));
#endif

    self->context.psr = 0; // clear the CPU flags
    self->context.gpr[0] = (PXX)arg; // pass the first argument to the thread
    self->context.lr = (U32) os_task_exit;
    self->context.sp = (U32) stack_ptr;
    self->context.pc = (U32) thread_main;

    if (!params)
    {
        params = &default_params;
    }

    // Store provided parameters
    FW_ASSERT(params->pri < OS_SCHED_PRI_N, params->pri);
    self->params = *params;
    self->state = TASK_READY;
    self->blocker = NULL;
    self->last_service = 0; // never been serviced

    // Add to linked list
    self->next = os_scheduler.task_rr_pri[params->pri];
    self->prev = NULL;

    if (self->next)
    {
        self->next->prev = self;
    }

    os_scheduler.task_rr_pri[params->pri] = self;
}

typedef struct {
    U32 r4;
    U32 r5;
    U32 r6;
    U32 r7;
    U32 r8;
    U32 r9;
    U32 r10;
    U32 r11;
    U32 xPSR;
    U32 lr;
    U32 r12;
    U32 r0;
    U32 r1;
    U32 r2;
    U32 r3;
    U32 pc;
} ISRStack;

static void os_task_save_ctx(const ISRStack* isr_stack)
{
    Task* task = os_current_task();
    task->context.gpr[0] = isr_stack->r0;
    task->context.gpr[1] = isr_stack->r1;
    task->context.gpr[2] = isr_stack->r2;
    task->context.gpr[3] = isr_stack->r3;
    task->context.gpr[4] = isr_stack->r4;
    task->context.gpr[5] = isr_stack->r5;
    task->context.gpr[6] = isr_stack->r6;
    task->context.gpr[7] = isr_stack->r7;
    task->context.gpr[8] = isr_stack->r8;
    task->context.gpr[9] = isr_stack->r9;
    task->context.gpr[10] = isr_stack->r10;
    task->context.gpr[11] = isr_stack->r11;
    task->context.gpr[12] = isr_stack->r12;

    task->context.lr = isr_stack->lr;
    task->context.psr = isr_stack->xPSR;
    task->context.sp = ((PXX)isr_stack) + sizeof(ISRStack);
    task->context.pc = isr_stack->pc;
}

void os_task_isr(const ISRStack* isr_stack)
{
    if (!scheduler_running)
    {
        return;
    }

    // Save the thread's state to the task
    os_task_save_ctx(isr_stack);

    // Schedule another task
    os_sched_next();
}

void os_sched_next(void)
{
    OS_CRITICAL_ENTER();

    Scheduler* self = &os_scheduler;

    // Tell the task about when it was serviced
    U32 curr_tim = tim_get();

    if (self->current_task)
    {
        self->current_task->last_service = curr_tim;
    }

    // Update all the task states
    for (U32 i = 0; i < OS_SCHED_PRI_N; i++)
    {
        for (Task* pri_curr = self->task_rr_pri[i]; pri_curr; pri_curr = pri_curr->next)
        {
            switch(pri_curr->state)
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
    if (!next_task)
    {
        next_task = &idle_task;
    }

    __asm__ volatile("cpsid I"); // disable interrupts
    OS_CRITICAL_EXIT();
    os_scheduler.current_task = next_task;
    __os_set_task_ctx(&next_task->context);
    FW_ASSERT(0 && "Failed to switch to task", next_task);
}

Task* os_current_task(void)
{
    return os_scheduler.current_task;
}

void os_sched_block(Event* self)
{
    Task* task = os_current_task();
    task->blocker = self;
    task->state = TASK_BLOCKED;

    __os_relinquish(&task->context);
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
    scheduler_running = 1;
    os_sched_next();
}
