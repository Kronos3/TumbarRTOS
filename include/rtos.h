//
// Created by tumbar on 10/16/21.
//

#ifndef TUMBARRTOS_RTOS_H
#define TUMBARRTOS_RTOS_H

#include <global.h>
#include <RTOSConfig.h>

typedef struct Task_prv Task;
typedef struct TaskParam_prv TaskParam;

/**
 * Create a new task
 * @param self Task block to initialize
 * @param params Task parameters (NULL if default)
 * @param stack_ptr Pointer to top of stack
 * @param thread_main Task execution pointer
 * @param arg Argument to pass to task
 */
void os_task_create(
        Task* self,
        const TaskParam* params,
        void* stack_ptr,
        void (*thread_main)(void*),
        void* arg);

/**
 * Get the current running task
 * @return running task
 */
Task* os_current_task(void);

#endif //TUMBARRTOS_RTOS_H
