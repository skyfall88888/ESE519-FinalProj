/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Output PWM signals on pins 0 and 1
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

#define PINS 29

int main() {
    stdio_init_all();
    gpio_init(PINS);
    gpio_set_dir(PINS, true);
    //while(stdio_usb_connected()!=true);
    int count1 = 0;
    int count2 = 0;

    while(true) {
        count1 = 0;
        count2 = 0;
        while (count1 < 100) {

            gpio_put(PINS, 1);
            sleep_ms(1);
            gpio_put(PINS, 0);
            sleep_ms(19);
            count1+= 1;
        }

            sleep_ms(2000);

        while(count2 < 100) {
            gpio_put(PINS, 1);
            sleep_ms(2);
            gpio_put(PINS, 0);
            sleep_ms(18);
            count2 += 1;
        }

        sleep_ms(2000);
    }
    return 0;
}
