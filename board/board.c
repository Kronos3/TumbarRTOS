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

#include "stm32f4xx.h"

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define  XTAL            ( 5000000UL)      /* Oscillator frequency */

#define  SYSTEM_CLOCK    (5U * XTAL)


/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = SYSTEM_CLOCK;

typedef enum
{
    HAL_TICK_FREQ_10HZ = 100U,
    HAL_TICK_FREQ_100HZ = 10U,
    HAL_TICK_FREQ_1KHZ = 1U,
    HAL_TICK_FREQ_DEFAULT = HAL_TICK_FREQ_1KHZ
} HAL_TickFreqTypeDef;

HAL_TickFreqTypeDef uwTickFreq = HAL_TICK_FREQ_DEFAULT;  /* 1KHz */
int uwTickPrio = 0;

#define IS_NVIC_PREEMPTION_PRIORITY(PRIORITY)  ((PRIORITY) < 0x10)

#define IS_NVIC_SUB_PRIORITY(PRIORITY)         ((PRIORITY) < 0x10)

void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{
    uint32_t prioritygroup = 0x00U;

    /* Check the parameters */
    FW_ASSERT(IS_NVIC_SUB_PRIORITY(SubPriority));
    FW_ASSERT(IS_NVIC_PREEMPTION_PRIORITY(PreemptPriority));

    prioritygroup = NVIC_GetPriorityGrouping();

    NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
}


static int init_tick(int TickPriority)
{
    /* Check uwTickFreq for MisraC 2012 (even if uwTickFreq is a enum type that doesn't take the value zero)*/
    if ((uint32_t) uwTickFreq != 0U)
    {
        /*Configure the SysTick to have interrupt in 1ms time basis*/
        if (SysTick_Config(SystemCoreClock / (1000U / (uint32_t) uwTickFreq)) == 0U)
        {
            /* Configure the SysTick IRQ priority */
            if (TickPriority < (1UL << __NVIC_PRIO_BITS))
            {
                HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority, 0U);
                uwTickPrio = TickPriority;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }

    return 0;
}

#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#define __HAL_RCC_SYSCFG_CLK_ENABLE()          do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define __HAL_RCC_PWR_CLK_ENABLE()             do { \
                                                 __IO uint32_t tmpreg; \
                                                 SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN); \
                                                 /* Delay after an RCC peripheral clock enabling */ \
                                                 tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN); \
                                                 UNUSED(tmpreg); \
                                               } while(0)

#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */

#define IS_NVIC_PRIORITY_GROUP(GROUP) (((GROUP) == NVIC_PRIORITYGROUP_0) || \
                                       ((GROUP) == NVIC_PRIORITYGROUP_1) || \
                                       ((GROUP) == NVIC_PRIORITYGROUP_2) || \
                                       ((GROUP) == NVIC_PRIORITYGROUP_3) || \
                                       ((GROUP) == NVIC_PRIORITYGROUP_4))

void HAL_NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
    /* Check the parameters */
    FW_ASSERT(IS_NVIC_PRIORITY_GROUP(PriorityGroup));

    /* Set the PRIGROUP[10:8] bits according to the PriorityGroup parameter value */
    NVIC_SetPriorityGrouping(PriorityGroup);
}

void board_init(void)
{
    /* Set Interrupt Group Priority */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* Use SysTick as time base source and configure 1ms tick (default clock after Reset is MSI) */
    int i_tick_st = init_tick(0);
    FW_ASSERT(i_tick_st == 0);

    /* Init the low level hardware */
//    HAL_MspInit();
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
}
