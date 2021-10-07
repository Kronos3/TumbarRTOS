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

extern void* memset(void* s, int c, int n)
{
    void* s_ = s;
    if ((n & 0x3) == 0)
    {
        while (n)
        {
            *(int*)s++ = c;
            n -= 4;
        }
    }
    else
    {
        while (n--)
        {
            *(char*)s++ = c;
            n -= 4;
        }
    }

    return s_;
}
