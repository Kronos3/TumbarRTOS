//
// Created by tumbar on 10/16/21.
//

#include <tim.h>

static U64 os_tick_ms = 0;

void tim_isr(void)
{
    os_tick_ms++;
}

U32 tim_get(void)
{
    return os_tick_ms;
}
