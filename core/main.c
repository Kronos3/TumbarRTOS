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

static char temp_stack[1024*512];

static void thread_main(void)
{
    while(1);
}

void os_main(void)
{
    Task s;
    os_task_create(&s, NULL, temp_stack, (void (*)(void*)) thread_main, NULL);
    while(1);
}
