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
#include <string.h>

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

void os_task_create(
        Task* self,
        const TaskParam* params,
        void* stack_ptr,
        void (* thread_main)(void*),
        void* arg)
{
    // Clear the register files
//    memset(self->gpr, 0, sizeof(self->gpr));

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
    FW_ASSERT_N(params->pri < OS_SCHED_PRI_N, params->pri);
    self->params = *params;

    // Add to linked list
    // TODO(tumbar) Allow task priority change - make doubly linked
    self->sched_next = os_scheduler.task_rr_pri[params->pri];
    os_scheduler.task_rr_pri[params->pri] = self;
}
