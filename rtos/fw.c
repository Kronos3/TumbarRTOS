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
#include <stdarg.h>
#include <stdio.h>

void fw_assertion_failure(const char* file, U32 line,
                          const char* expr_str,
                          U32 nargs, ...)
{
    printf("Assertion failed %s:%d : %s",
           file, line, expr_str);

    va_list args;
    va_start(args, nargs);
    for (U32 i = 0; i < nargs; i++)
    {
        printf(", %d", va_arg(args, int));
    }
    va_end(args);
    printf("\r\n");

    // Hang Mr. CPU please
    // Flash LEDs using TIM5
    __asm__ volatile("bkpt \n"
                     "cpsid i");
    for (;;);
}

typedef struct {
    U32 r0;
    U32 r1;
    U32 r2;
    U32 r3;
    U32 r12;
    U32 lr;
    U32 pc;
    U32 xpsr;
} ContextFrame;


void load_context_from_stack(const ContextFrame* ctx)
{
    printf("\r\nHardfault:\r\n"
            "  r0: 0x%08x\r\n"
            "  r1: 0x%08x\r\n"
            "  r2: 0x%08x\r\n"
            "  r3: 0x%08x\r\n"
            " r12: 0x%08x\r\n"
            "  lr: 0x%x\r\n"
            "  pc: 0x%x\r\n"
            "xpsr: 0x%x\r\n",
            ctx->r0,
            ctx->r1,
            ctx->r2,
            ctx->r3,
            ctx->r12,
            ctx->lr,
            ctx->pc,
            ctx->xpsr);

    FW_ASSERT(0 && "Hard-fault error pc:",
              ctx->pc);
}
