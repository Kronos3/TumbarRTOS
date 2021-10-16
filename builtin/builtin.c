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

typedef unsigned long long uint64_t;
typedef long long int64_t;

extern void* memset(void* s, int c, int n)
{
    void* s_ = s;
    if ((n & 0x3) == 0)
    {
        while (n)
        {
            *(int*) s++ = c;
            n -= 4;
        }
    }
    else
    {
        while (n--)
        {
            *(char*) s++ = c;
            n -= 4;
        }
    }

    return s_;
}

uint64_t __udivmoddi4(uint64_t num, uint64_t den, uint64_t* rem_p)
{
    uint64_t quot = 0, qbit = 1;

    if (den == 0)
    {
        return 1 / ((unsigned) den); /* Intentional divide by zero, without
                                  triggering a compiler warning which
                                  would abort the build */
    }

/* Left-justify denominator and count shift */
    while ((int64_t) den >= 0)
    {
        den <<= 1;
        qbit <<= 1;
    }

    while (qbit)
    {
        if (den <= num)
        {
            num -= den;
            quot += qbit;
        }
        den >>= 1;
        qbit >>= 1;
    }

    if (rem_p)
    {
        *rem_p = num;
    }

    return quot;
}
