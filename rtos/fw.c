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

#include <global.h>
#include <scheduler.h>
#include <stdarg.h>
#include <io.h>

void fw_assertion_failure(const char* file, U32 line,
                          const char* expr_str,
                          U32 nargs, ...)
{
    uprintf("Assertion failed %s:%d : (%s) == 0",
            file, line, expr_str);

    va_list args;
    va_start(args, nargs);
    for (U32 i = 0; i < nargs; i++)
    {
        uprintf(", %d", va_arg(args, int));
    }
    va_end(args);
    uprintf("\r\n");

    // Hang Mr. CPU please
    // Flash LEDs using TIM5
    __asm__ volatile("bkpt");
    while (1)
    {
        // Create a nice little flashing pattern
        // This cannot be mistaken for some status code
//        set_led_1(TIM5->CNT & (1 << 11));
//        set_led_2(!(TIM5->CNT & (1 << 11)));
//        set_led_3(!(TIM5->CNT & (1 << 11)));
//        set_led_4(TIM5->CNT & (1 << 11));
    }
}

static volatile TaskContext temp_ctx;
void load_context_from_stack(const U32* pulFaultStackAddress)
{
    temp_ctx.gpr[0] = pulFaultStackAddress[0];
    temp_ctx.gpr[1] = pulFaultStackAddress[1];
    temp_ctx.gpr[2] = pulFaultStackAddress[2];
    temp_ctx.gpr[3] = pulFaultStackAddress[3];

    temp_ctx.gpr[12] = pulFaultStackAddress[4];
    temp_ctx.lr = pulFaultStackAddress[5];
    temp_ctx.pc = pulFaultStackAddress[6];
    temp_ctx.psr = pulFaultStackAddress[7];
}

void HardFault_Handler()
{
    __asm volatile
    (
    " tst lr, #4                                                \n"
    " ite eq                                                    \n"
    " mrseq r0, msp                                             \n"
    " mrsne r0, psp                                             \n"
    " ldr r1, [r0, #24]                                         \n"
    " bl load_context_from_stack                                \n"
    );
    FW_ASSERT(0 && "Hard-fault error");
}

void MemManage_Handler()
{
    FW_ASSERT(0 && "MemManage error");
}

void BusFault_Handler()
{
    FW_ASSERT(0 && "Hard-fault error");
}

void UsageFault_Handler()
{
    FW_ASSERT(0 && "Usage-fault error");
}
