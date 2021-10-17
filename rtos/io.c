#include <io.h>
#include <stdarg.h>

int __io_putchar(int ch)
{
    usart_putc(ch);
    return 1;
}

int __io_getchar(void)
{
    return usart_getc();
}

static inline void printf_string(const char* string)
{
    for (; *string; string++)
    {
        usart_putc(*string);
    }
}

static inline void usart_put_udec(unsigned i, char pad_char, U8 pad_amt)
{
    char buf_[10];
    char* stack_position = buf_;
    if (!i)
    {
        *(stack_position++) = '0';
    }

    while (i)
    {
        *(stack_position++) = '0' + (i % 10);
        i /= 10;
    }

    if ((stack_position - buf_) < pad_amt)
    {
        for (U32 j = 0; j < (pad_amt - (stack_position - buf_)); j++)
        {
            usart_putc(pad_char);
        }
    }

    while (stack_position != buf_)
    {
        usart_putc(*(--stack_position));
    }
}

static inline void usart_put_dec(int i, char pad_char, U8 pad_amt)
{
    if (i < 0)
    {
        usart_putc('-');
        i *= -1;
    }

    usart_put_udec(i, pad_char, pad_amt);
}

static inline void usart_put_hex(U32 x, char base_hex_a, char pad_char, U8 pad_amt)
{
    I32 printed = 0;
    for (U32 i = 0; i < 8; i++)
    {
        U32 j = (x >> 28) & 0xF;
        if (j >= 0xA)
        {
            printed = 1;
            usart_putc(base_hex_a + (j - 0xA));
        }
        else if (j == 0 && !printed && i != 7)
        {
            if ((8 - i - 1) < pad_amt)
            {
                usart_putc(pad_char);
            }
        }
        else
        {
            printed = 1;
            usart_putc('0' + j);
        }

        x <<= 4;
    }
}

I32 uprintf(const char * const format, ...)
{
    va_list args;
    va_start(args, format);

    const char* iter = format;
    while (*iter)
    {
        if (*iter == '%')
        {
            char pad_char = 0;
            U8 pad_amt = 0;

            iter++;
            switch(*iter)
            {
                case 's':
                    printf_string(va_arg(args, const char*));
                    break;
                case 'c':
                    usart_putc(va_arg(args, int));
                    break;
                case 'd':
                case 'i':
                    usart_put_dec(va_arg(args, int), pad_char, pad_amt);
                    break;
                case 'u':
                    usart_put_udec(va_arg(args, unsigned), pad_char, pad_amt);
                    break;
                case 'x':
                    usart_put_hex(va_arg(args, int), 'a', pad_char, pad_amt);
                    break;
                case 'X':
                    usart_put_hex(va_arg(args, int), 'A', pad_char, pad_amt);
                    break;
                default:
                    pad_char = *(iter++);
                    pad_amt = (*(iter++) - '0');
                    {
                        if (*iter == 'd' || *iter == 'i')
                        {
                            usart_put_dec(va_arg(args, int), pad_char, pad_amt);
                        }
                        else if (*iter == 'u')
                        {
                            usart_put_udec(va_arg(args, int), pad_char, pad_amt);
                        }
                        else if (*iter == 'x')
                        {
                            usart_put_hex(va_arg(args, int), 'a', pad_char, pad_amt);
                        }
                        else if (*iter == 'x')
                        {
                            usart_put_hex(va_arg(args, int), 'A', pad_char, pad_amt);
                        }
                    }
                    FW_ASSERT(pad_amt <= 9, pad_amt);
                    break;
            }
        }
        else
        {
            usart_putc(*iter);
        }

        iter++;
    }

    va_end(args);

    usart_flush();
    return iter - format;
}

void uclear(void)
{
    static const U8 clear_screen_bytes[] = {
            0x1B, 0x5B, 0x32, 0x4A,     // Clear the screen
            0x1B, 0x5B, 0x48,           // Move cursor to home
    };

    for (U32 i = 0; i < sizeof(clear_screen_bytes); i++)
    {
        usart_putc(clear_screen_bytes[i]);
    }
}
