//
// Created by tumbar on 10/16/21.
//

#ifndef TUMBARRTOS_IO_H
#define TUMBARRTOS_IO_H

#include <global.h>

// Read and write to the same UART device
#define u_stdout USART2
#define u_stdin USART2

#define BufferSize 50

// HAL definitions
#define GPIO_SPEED_FREQ_VERY_HIGH    (0x00000003u)    /*!< range 50 MHz to 80 MHz, please refer to the product datasheet */
#define GPIO_AF7_USART2                ((uint8_t)0x07) /*!< USART2 Alternate Function mapping     */
#define GPIO_MODE_AF_PP             (0x00000002u)   /*!< Alternate Function Push Pull Mode     */
#define UART_WORDLENGTH_8B          0x00000000U     /*!< 8-bit long UART frame */
#define UART_PARITY_NONE            0x00000000U     /*!< No parity   */
#define UART_OVERSAMPLING_16        0x00000000U     /*!< Oversampling by 16 */
#define UART_STOPBITS_1             0x00000000U     /*!< UART frame with 1 stop bit    */
#define UART_HWCONTROL_NONE         0x00000000U     /*!< No hardware control       */

/**
 * Write a character to stdout (USART2)
 * @param c Character to write
 */
void usart_putc(char c);

/**
 * Read a character from stdin (USART2)
 * @return Character read from uart
 */
char usart_getc(void);

/**
 * Wait until the UART finishes transmitting
 */
void usart_flush(void);

/**
 * Printf to uart
 * @param format_str format string
 * @param ... printf va_args
 * @return length of write
 */
I32 uprintf(const char* format_str, ...);

#endif //TUMBARRTOS_IO_H
