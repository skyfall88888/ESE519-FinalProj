/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"

const uint tb1_PIN = 29 ;  //A0
const uint tb2_PIN = 28 ;  //A1
const uint pwm1_PIN = 27;  //A2
const uint pwm2_PIN = 26;  //A3


void init_motor_6612(){
    gpio_init(tb1_PIN);
    gpio_init(tb2_PIN);
    gpio_init(pwm1_PIN);
    gpio_init(pwm2_PIN);


    gpio_set_dir(tb1_PIN, GPIO_OUT);
    gpio_set_dir(tb2_PIN, GPIO_OUT);
    gpio_set_dir(pwm1_PIN, GPIO_OUT);
    gpio_set_dir(pwm2_PIN, GPIO_OUT);

}


int main() {
    
    init_motor_6612();
    gpio_put(tb1_PIN, 1); 
    gpio_put(tb2_PIN, 0);
    gpio_put(pwm1_PIN, 1); 
    gpio_put(pwm2_PIN, 0);


    // while (true) {
    //     // gpio_put(pwm1_PIN, 1); 
    //     // sleep_ms();
    // }
}
