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

#ifndef __TUMBAR_RTOS__
#error "__TUMBAR_RTOS__ must be linked to use this header"
#endif

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef unsigned long long U64;

typedef char I8;
typedef short I16;
typedef int I32;
typedef long long I64;

typedef float F32;
typedef double F64;

typedef U32 PXX;
typedef I32 IXX;

typedef enum {
    FALSE = 0,
    TRUE = 1,
} bool_t;

#ifndef NULL
#define NULL ((void*) (0))
#endif

#ifndef offsetof
#define offsetof(st, m) __builtin_offsetof(st, m)
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

#define FW_ASSERT(expr, ...) do {                \
    if (!(expr)) fw_assertion_failure(__FILE__, __LINE__, #expr, COUNT_ARGUMENTS(__VA_ARGS__), ##__VA_ARGS__);   \
} while(0)

#define DISABLE_INTERRUPTS() __asm__ volatile("cpsid I")
#define ENABLE_INTERRUPTS() __asm__ volatile("cpsie I")

COMPILE_ASSERT(sizeof(IXX) == sizeof(PXX), sizeof_ixx_pxx);
COMPILE_ASSERT(sizeof(void*) == sizeof(PXX), sizeof_ptr); // enforce 32-bit target
COMPILE_ASSERT(sizeof(I8) == 1, sizeof_u8);
COMPILE_ASSERT(sizeof(U8) == 1, sizeof_i8);
COMPILE_ASSERT(sizeof(U16) == 2, sizeof_u16);
COMPILE_ASSERT(sizeof(I16) == 2, sizeof_i16);
COMPILE_ASSERT(sizeof(U32) == 4, sizeof_u32);
COMPILE_ASSERT(sizeof(I32) == 4, sizeof_i32);
COMPILE_ASSERT(sizeof(U64) == 8, sizeof_u64);
COMPILE_ASSERT(sizeof(I64) == 8, sizeof_i64);
COMPILE_ASSERT(sizeof(F32) == 4, sizeof_f32);
COMPILE_ASSERT(sizeof(F64) == 8, sizeof_f64);

#endif //TUMBARRTOS_GLOBAL_H
