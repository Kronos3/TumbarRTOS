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

#include <schedule.h>
//#include <string.h>

static const TaskParam default_params = {
        .pri = 15, // medium priority
};

static Scheduler os_scheduler = {
        .task_rr_pri = {NULL}
};

static const Task volatile* current_task;

/**
 * Private function that will execute after
 * the execution of a thread has finished
 */
static void os_task_exit(void)
{
    // Remove this task from execution

}

void __os_set_task_ctx(Task*);
void* memset(void* s, int c, int n);

void os_task_create(
        Task* self,
        const TaskParam* params,
        void* stack_ptr,
        void (* thread_main)(void*),
        void* arg)
{
    // Clear the register files
    memset(self->gpr, 0, sizeof(self->gpr));

#ifdef M4_FPU
    memset(self->gpr, 0, sizeof(self->spr));
    memset(self->gpr, 0, sizeof(self->dpr));
#endif

    self->psr = 0; // clear the CPU flags
    self->gpr[0] = (PXX)arg; // pass the first argument to the thread
    self->lr = (U32) os_task_exit;
    self->sp = (U32) stack_ptr;
    self->pc = (U32) thread_main;

    if (!params)
    {
        params = &default_params;
    }

    // Store provided parameters
    FW_ASSERT(params->pri < OS_SCHED_PRI_N, params->pri);
    self->params = *params;
    self->running = 1;

    // Add to linked list
    // TODO(tumbar) Allow task priority change - make doubly linked
    self->pri_next = os_scheduler.task_rr_pri[params->pri];
    self->sched_next = NULL;
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
    task->gpr[0] = isr_stack->r0;
    task->gpr[1] = isr_stack->r1;
    task->gpr[2] = isr_stack->r2;
    task->gpr[3] = isr_stack->r3;
    task->gpr[4] = isr_stack->r4;
    task->gpr[5] = isr_stack->r5;
    task->gpr[6] = isr_stack->r6;
    task->gpr[7] = isr_stack->r7;
    task->gpr[8] = isr_stack->r8;
    task->gpr[9] = isr_stack->r9;
    task->gpr[10] = isr_stack->r10;
    task->gpr[11] = isr_stack->r11;
    task->gpr[12] = isr_stack->r12;

    task->lr = isr_stack->lr;
    task->psr = isr_stack->xPSR;
    task->sp = ((PXX)isr_stack) + sizeof(ISRStack);
    task->pc = isr_stack->pc;
}

void os_task_isr(const ISRStack* isr_stack)
{
    // Save the thread's state to the task
    os_task_save_ctx(isr_stack);


}

void os_sched_next(void)
{
    FW_ASSERT(os_scheduler.current_task && "Deadlock reached!");

    if ()
}

void os_schedule_task(Task* self)
{

}

Task* os_current_task(void)
{
    return os_scheduler.current_task;
}
