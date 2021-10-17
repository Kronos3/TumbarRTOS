//
// Created by tumbar on 10/17/21.
//

#ifndef TUMBARRTOS_LED_H
#define TUMBARRTOS_LED_H

#include <global.h>

/**
 * Initialize the GPIO pins for LED usage
 */
void gpio_led_init(void);

// LED control interface
void set_led_1(U32 on);
void set_led_2(U32 on);
void set_led_3(U32 on);
void set_led_4(U32 on);
void set_led_mask(U32 mask);

void seven_segment_set(U32 number);

#endif //TUMBARRTOS_LED_H
