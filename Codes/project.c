// project.c
#include "servo.h"
#include "sound.h"

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/stdio.h"
#include "hardware/pwm.h"

#define PINS 29         //A0---servo

int main() {

    stdio_init_all();

    adc_init();
    adc_gpio_init(27);
    adc_select_input(1);

    gpio_init(PINS);
    gpio_set_dir(PINS, true);
    
    int sound_result;

    while (true) {
        //("in main\n");
        sound_result = get_sound();
        if (sound_result == 1) {
            //printf("main get sound\n");
            rotate();
            //printf("back from rotate\n");
        }
    }
}