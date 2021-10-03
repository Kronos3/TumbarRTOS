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

void fw_assertion_failure(const char* file, U32 line,
                          const char* expr_str,
                          U32 nargs, ...)
{
//    uprintf("Assertion failed %s:%d : (%s) == 0",
//            file, line, expr_str);

    va_list args;
    va_start(args, nargs);
    for (U32 i = 0; i < nargs; i++)
    {
//        uprintf(", %d", va_arg(args, int));
    }
    va_end(args);
//    uprintf("\r\n");

    // Hang Mr. CPU please
    // Flash LEDs using TIM5
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

