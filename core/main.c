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

#include <scheduler.h>

STACK_WORDS(test_stack, 128);

static void thread_main(void)
{
    while(1);
}


void board_init(void);
void os_main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    board_init();

    Task s;
    os_task_create(&s, NULL, test_stack, (void (*)(void*)) thread_main, NULL);

    os_scheduler_main();
}
