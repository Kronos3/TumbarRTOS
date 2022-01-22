//
// Created by tumbar on 10/16/21.
//

#include <semaphore.h>

static void semaphore_event_fire(Task* self)
{
    // Move all tasks blocking on this to READY
    // Only the next task that executes will be able
    // to take the ownership of the semaphore
    self->state = TASK_READY;

    // Clear the event (only fire once)
    os_sched_event_clear(self->blocker);

    // Semaphore is no longer blocked by this semaphore
    self->blocker = NULL;
}

void semaphore_binary_create(Semaphore* self)
{
    semaphore_counting_create(self, 1);
}

void semaphore_counting_create(Semaphore* self, U32 count)
{
    FW_ASSERT(self);
    if (os_sched_alloc_event(&self->super))
    {
        FW_ASSERT(0 && "Failed to allocate event, increase OS_MAX_EVENT?");
    }

    self->super.event_handler = semaphore_event_fire;
    self->init_counter = count;
    self->counter = count;
}

I32 semaphore_try_take__(Semaphore* self)
{
    if (self->counter)
    {
        self->counter--;
        return 1;
    }
    else
    {
        return 0;
    }
}

void semaphore_take__(Semaphore* self)
{
    // While loop because this task will
    // unblock when another task gives back the semaphore
    // however, it could have already been taken by another task
    // Keep blocking until
    while (!self->counter)
    {
        // We can't take the semaphore
        // Block the task
        os_sched_block(&self->super);
    }

    // We can take the semaphore
    self->counter--;
}

void semaphore_give__(Semaphore* self)
{
    FW_ASSERT(self->counter < self->init_counter && "Too many SEM_GIVE()",
              self, self->counter, self->init_counter);

    self->counter++;

    // Signal the scheduler that an event occurred
    os_sched_event_fire(&self->super);
}
