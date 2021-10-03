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


#ifndef TUMBARRTOS_GLOBAL_H
#define TUMBARRTOS_GLOBAL_H

#include <stdint.h>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;

typedef float F32;
typedef double F64;

// TODO(tumbar) Figure out why clangd is not detecting the change (uintptr_t)
typedef U32 PXX;
typedef I32 IXX;

#ifndef NULL
#define NULL ((void*) (0))
#endif

#define COMPILE_ASSERT(expr, name) typedef char __compile_assert_##name[(expr) ? 0 : -1]

/**
 * Handle assertion failures
 * Hang the cpu and flash all of the LEDs
 * @param file File where assertion failure occurred
 * @param line Line of assertion failure
 * @param expr_str Asserted expression failure
 * @param nargs number of arguments to print
 * @param ... arguments to print
 */
__attribute__((noreturn)) void fw_assertion_failure(const char* file, U32 line, const char* expr_str, U32 nargs, ...);

#define ELEVENTH_ARGUMENT(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, ...) a11
#define COUNT_ARGUMENTS(...) ELEVENTH_ARGUMENT(dummy, ## __VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define FW_ASSERT_N(expr, ...) do {                \
    if (!(expr)) fw_assertion_failure(__FILE__, __LINE__, #expr, COUNT_ARGUMENTS(__VA_ARGS__), ##__VA_ARGS__);   \
} while(0)

#define FW_ASSERT(expr) do {                \
    if (!(expr)) fw_assertion_failure(__FILE__, __LINE__, #expr, 0);   \
} while(0)

#endif //TUMBARRTOS_GLOBAL_H
