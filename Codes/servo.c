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

#define PINS 29         //A0

void rotate() {
    // stdio_init_all();
    // gpio_init(PINS);
    // gpio_set_dir(PINS, true);
    //while(stdio_usb_connected()!=true);
    int count1 = 0;
    int count2 = 0;

    //while(true) {
        // count1 = 0;
        // count2 = 0;
        //printf("servo start\n");
        while (count1 < 50) {
            //printf("rotate dir 1\n");
            gpio_put(PINS, 1);
            sleep_ms(1);
            gpio_put(PINS, 0);
            sleep_ms(29);
            count1+= 1;
        }

            sleep_ms(20);

        while(count2 < 50) {
            //printf("rotate dir 2\n");
            gpio_put(PINS, 1);
            sleep_ms(2);
            gpio_put(PINS, 0);
            sleep_ms(28);
            count2 += 1;
        }

        sleep_ms(20);
        //printf("servo done\n");
    //}
}

    
    



