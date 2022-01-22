//
// Created by tumbar on 10/16/21.
//

#ifndef TUMBARRTOS_TIM_H
#define TUMBARRTOS_TIM_H

#include <global.h>

/**
 * Increment the system time
 * Called by SysTick_Handler
 */
void tim_isr(void);

U32 tim_get(void);

#endif //TUMBARRTOS_TIM_H
