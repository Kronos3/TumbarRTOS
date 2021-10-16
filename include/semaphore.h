//
// Created by tumbar on 10/16/21.
//

#ifndef TUMBARRTOS_SEMAPHORE_H
#define TUMBARRTOS_SEMAPHORE_H

#include <global.h>
#include <scheduler.h>

typedef struct Semaphore_prv Semaphore;

struct Semaphore_prv
{
    Event super;

    volatile U32 counter;
    U32 init_counter;       //!< Maximum times give() can be called
};

/**
 * Create a binary semaphore (mutex)
 * @param self Semaphore to initialize
 */
void semaphore_binary_create(Semaphore* self);

/**
 * Create a counting semaphore
 * @param self Semaphore to initialize
 * @param count Count to start semaphore at
 */
void semaphore_counting_create(Semaphore* self, U32 count);

/**
 * Attempt to take the semaphore
 * @param self Semaphore to take
 * @return 0 if not taken, 1 if taken
 */
I32 semaphore_try_take__(Semaphore* self);

/**
 * Take a semaphore, block until able to take
 * @param self Semaphore to take
 */
void semaphore_take__(Semaphore* self);

/**
 *
 * @param self
 */
void semaphore_give__(Semaphore* self);

#define SEM_TRY_TAKE(self__) OS_CRITICAL(semaphore_try_take__((self)))
#define SEM_TAKE(self__) OS_CRITICAL(semaphore_take__((self)))
#define SEM_GIVE(self__) OS_CRITICAL(semaphore_give__((self)))

#endif //TUMBARRTOS_SEMAPHORE_H
