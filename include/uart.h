//
// Created by tumbar on 10/16/21.
//

#ifndef TUMBARRTOS_UART_H
#define TUMBARRTOS_UART_H

#include <global.h>

/**
 * Printf to uart
 * @param format_str format string
 * @param ... printf va_args
 * @return length of write
 */
I32 uprintf(const char* format_str, ...);

#endif //TUMBARRTOS_UART_H
