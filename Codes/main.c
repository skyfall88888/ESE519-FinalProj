/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "adps.h"
#include "adps.c"
#include "DCmotor.c"

#include <stdio.h>
#include <stdlib.h>


int main() {
    int32_t proximity;

    stdio_init_all();


    // while(stdio_usb_connected()!=true);
    sleep_ms(2000);

    printI2Cbus();
    init_motor_6612();
    
    //set up motor driver
    gpio_put(tb1_PIN, 1); 
    gpio_put(tb2_PIN, 0);
    gpio_put(pwm1_PIN, 1); 
    gpio_put(pwm2_PIN, 0); 

int t = 0;

while(t < 4){
    read_proximity(&proximity);
    // printf("distance:%d\n", proximity); 

    if(proximity > 200){
        gpio_put(pwm1_PIN, 0);  //if distance is very close, let the car turn to another direction
        gpio_put(pwm2_PIN, 1);
        sleep_ms(1500);
        gpio_put(pwm1_PIN, 1); 
        gpio_put(pwm2_PIN, 0);
        t += 1;
    }
    
}

gpio_put(pwm1_PIN, 0); 
gpio_put(pwm2_PIN, 0); 
    
}
