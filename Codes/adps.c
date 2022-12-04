#include <stdio.h>

#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "neopixel.h"
#include "pio_i2c.h"
// #include "adps.h"

#define GEN 0 //gesture enable 
#define AEN 1 //ALS Enable. 
#define PON 1 //Power ON. 
#define PEN 1 //Proximity enable.

#define CONFIGURATION (GEN << 6U) | (PEN << 2u) | (AEN << 1u) | PON

uint8_t buf[2];  //store commands

int returned;  //record r/w status
uint8_t first;
uint8_t last;


PIO pio = pio0;
uint sm = 1;

bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

int get_val(int a, int b) {
    int val;
    if (a == 0){    // from first: 0000xxxx
        val = (first >> (3-b));
    }
    else {  // from last: 0000xxxx
        val = (last >> (3-b));
    }
}

int apds_init() {

    buf[0] = enable_addr;
    buf[1] = CONFIGURATION;
    returned = pio_i2c_write_blocking(pio, sm, addr, buf, 2, false); // true to keep master control of bus
    if (returned ==  PICO_ERROR_GENERIC) {
        printf("ERROR: PON not written\n");
        return -1;
    }

    buf[0] = ATIME;
    buf[1] = ALS_TIME;
    pio_i2c_write_blocking(pio, sm, addr, buf, 2, false);

    return 1;
    
}

int set_gmode(int gmode_val){
     // config gmode
    buf[0] = gmode_addr;
    buf[1] = gmode_val;  
    returned = pio_i2c_write_blocking(pio, sm, addr, buf, 2,false); // true to keep master control of bus
    if (returned ==  PICO_ERROR_GENERIC) {
        printf("ERROR: GMODE not written\n");
        return -2;
    }
}

int set_g_exit_thredshold(int thredshold){
    buf[0] = GEXTH;
    buf[1] = thredshold;  
    returned = pio_i2c_write_blocking(pio, sm, addr, buf, 2,false); // true to keep master control of bus
    if (returned ==  PICO_ERROR_GENERIC) {
        printf("ERROR: GEXTH not written\n");
        return -2;
    }

}

void read_proximity(int32_t* proximity) {

    uint8_t buf[1];
    uint8_t reg = PDATA;
    pio_i2c_write_blocking(pio, sm, addr, &reg, 1, true);  // true to keep master control of bus
    pio_i2c_read_blocking(pio, sm, addr, buf, 1, false);  // false - finished with bus

    *proximity = buf[0];
}

static void read_prox() {
    uint8_t proxi;

    printf("read from buffer-c\n");
    pio_i2c_write_blocking(pio, sm, addr, &PDATA, 1,true);
    pio_i2c_read_blocking(pio, sm, addr, &proxi, 1, false);

    printf("distance: %u\n", proxi);
   
}

void read_rgbc(int32_t* r, int32_t* g, int32_t* b, int32_t* c) {

    uint8_t buf[8];
    uint8_t reg = clear_low;
    returned = pio_i2c_write_blocking(pio, sm, addr, &reg, 1, true);  // true to keep master control of bus
    // printf("write rgbc status: %d\n", returned);
    returned = pio_i2c_read_blocking(pio, sm, addr, buf, 8, false);  // false - finished with bus
    // printf("read rgbc status: %d\n", returned);


    *c = (buf[1] << 8) | buf[0];
    *r = (buf[3] << 8) | buf[2];
    *g = (buf[5] << 8) | buf[4];
    *b = (buf[7] << 8) | buf[6];
}
void clear_gesture_fifo(){
    //clear the 32 dataset fifo
    int room = 32;  
    uint8_t buf[4];
    while(room > 0){
        returned = pio_i2c_write_blocking(pio, sm, addr, &ges_addr, 1, true);  // true to keep master control of bus
        returned = pio_i2c_read_blocking(pio, sm, addr, buf, 4, false);  // read a group of gesture data
        room = room - 1;
    }       
}

int read_gesture() {
    uint8_t clear_top = 0x0F;

    uint8_t up_mask_1 = 0x80;
    uint8_t down_mask_1 = 0x40;
    uint8_t left_mask_1 = 0x20;
    uint8_t right_mask_1 = 0x10;

    uint8_t up_mask_2 = 0x08;
    uint8_t down_mask_2 = 0x04;
    uint8_t left_mask_2 = 0x02;
    uint8_t right_mask_2 = 0x01;

    uint8_t up_data_8b;
    uint8_t down_data_8b;
    uint8_t left_data_8b;
    uint8_t right_data_8b;

    uint8_t combined;
    uint8_t combined4_1;
    uint8_t combined4_2;


    uint8_t mot_buf[4];
    uint8_t n_to_read = 0;
    uint8_t ges_status;
    int wait_count = 0;

    signed int delta_lr;
    signed int delta_ud;

    signed int f_r_ud;
    signed int f_r_lr;
    signed int l_r_ud;
    signed int l_r_lr;
    signed int state_ud;
    signed int state_lr;
    int gesture_found = 0;
    int count = 0;

    returned = set_gmode(1);
    // set_g_exit_thredshold(0);

    // If FIFOs have overflowed we're already way too late, so clear those FIFOs and wait
    pio_i2c_write_blocking(pio, sm, addr, &GSTATUS, 1, true); 
    pio_i2c_read_blocking(pio, sm, addr, &ges_status, 1, false); 
    if((ges_status & 0x02) == 0x02  )  {  // <1> 1--filled;  
    //clear FIFO
        clear_gesture_fifo();
        printf("FIFO is cleaned.\n");
    } 

    printf("Detecting gesture...\n");
    while(n_to_read == 0 && wait_count < 10 ){  
        printf("awaiting gesture\n");
        returned = pio_i2c_write_blocking(pio, sm, addr, &ges_level_addr, 1, true);  
        // printf("write gesture status: %d\n", returned);
        returned = pio_i2c_read_blocking(pio, sm, addr, &n_to_read, 1, false);  // see how many dataset left to read
        //printf("Detecting gesture...\n");
        wait_count += 1;
    }

    //printf("%d gesture left to read.\n", n_to_read);

    while(n_to_read > 0){
        returned = pio_i2c_write_blocking(pio, sm, addr, &ges_addr, 1, true);  // true to keep master control of bus
        // printf("write gesture status: %d\n", returned);
        returned = pio_i2c_read_blocking(pio, sm, addr, mot_buf, 4, false);  // read a group of gesture data

        if (count == 0) {
            //printf("setting first buffer data, mot_buff[absolute 1st]: %x\n", mot_buf[0]);
            up_data_8b = mot_buf[0] & up_mask_1;
            first = (up_data_8b >> 4) & clear_top;
            down_data_8b = mot_buf[0] & down_mask_1;
            first = first | ((down_data_8b >> 4) & clear_top);

            left_data_8b = mot_buf[0] & left_mask_1;
            first = first | ((left_data_8b >> 4) & clear_top);

            right_data_8b = mot_buf[0] & right_mask_1;
            first = first | ((right_data_8b >> 4) & clear_top);
            //printf("first buff has been set to: %x\n", first);
        }

        for (int i = 0; i < 4; i++) {
            //printf("mot_buf[%d] is: %x\n", i, mot_buf[i]);


            //for first 4 bits of mot_buff
            //printf("upper half set, mot_buf[%d] is:%x\n", i, mot_buf[i]);
            up_data_8b = mot_buf[i] & up_mask_1;
            last = (up_data_8b >> 4);
            down_data_8b = mot_buf[i] & down_mask_1;
            last = last | (down_data_8b >> 4);

            left_data_8b = mot_buf[i] & left_mask_1;
            last = last | (left_data_8b >> 4);

            right_data_8b = mot_buf[i] & right_mask_1;
            last = last | (right_data_8b >> 4);
            //printf("last has been set to: %x\n", last);

            //for second 4 bits of mot_buf
            last = mot_buf[i] & up_mask_2;
            down_data_8b = mot_buf[i] & down_mask_2;
            last = last | (down_data_8b);

            left_data_8b = mot_buf[i] & left_mask_2;
            last = last | (left_data_8b);

            right_data_8b = mot_buf[i] & right_mask_2;
            last = last | (right_data_8b);
            //printf("lsat updated to: %x\n", last);

            // up_data_8b = mot_buf[i] & up_mask_1;
            // printf("%x\n",up_data_8b );
            // down_data_8b = mot_buf[i] & down_mask_1;
            //             printf("%x\n",down_data_8b );

            // left_data_8b = mot_buf[i] & left_mask_1;
            //             printf("%x\n",left_data_8b );

            // right_data_8b = mot_buf[i] & right_mask_1;
            //             printf("%x\n",right_data_8b );


            // combined4_1 = up_data_8b | down_data_8b | left_data_8b | right_data_8b;
            // printf("combined4_1 is: %x\n", combined4_1);

            // up_data_8b = mot_buf[i] & up_mask_2;
            //             printf("%x\n",up_data_8b );

            // down_data_8b = mot_buf[i] & down_mask_2;
            //                         printf("%x\n",down_data_8b );

            // left_data_8b = mot_buf[i] & left_mask_2;
            //                         printf("%x\n",left_data_8b );

            // right_data_8b = mot_buf[i] & right_mask_2;
            //                         printf("%x\n",right_data_8b );


            // combined4_2 = up_data_8b | down_data_8b | left_data_8b | right_data_8b;
            // combined = (combined4_1<<4) | (combined4_2);
            // printf("combined: %x\n", combined);

        }
        printf("current first: %x\n", first);
        printf("current last: %x\n", last);

        f_r_ud = ((get_val(0,0) - get_val(0,1) * 100));
        f_r_lr = ((get_val(0,2) - get_val(0,3) * 100));

        l_r_ud = ((get_val(1,0) - get_val(1,1) * 100)); 
        l_r_lr = ((get_val(1,2) - get_val(1,3) * 100));

        state_ud = 0;
        state_lr = 0;

        if (delta_ud >= 30){
            state_ud = 1;
        }
        else if  (delta_ud <= -30){
            state_ud = -1;
        }

        if (delta_lr >= 30){
            state_lr = 1;
        }
        else if (delta_lr <= -30){
            state_lr = -1;
        }

        if (state_ud == -1 && state_lr == 0){
            gesture_found = 1;
        }
        else if (state_ud == 1 && state_lr == 0){
            gesture_found = 2;
        }
        else if (state_ud == 0 && state_lr == -1){
            gesture_found = 3;
        }
        else if (state_ud == 0 && state_lr == 1){
            gesture_found = 4;
        }


        if (gesture_found == 0){
            if (state_ud == -1 && state_lr == 1){
                if (abs(delta_ud) > abs(delta_lr)){
                    gesture_found = 1;
                }
                else {                
                    gesture_found = 4;
                }
            }
            else if (state_ud == 1 && state_lr == -1){
                if (abs(delta_ud) > abs(delta_lr)){
                    gesture_found = 2;
                }
                else {
                    gesture_found = 3;
                }
            }

            else if (state_ud == -1 && state_lr == -1){
                if (abs(delta_ud) > abs(delta_lr)){
                    gesture_found = 1;
                }
                else {
                    gesture_found = 3;
                }
            }

            else if (state_ud == 1 && state_lr == 1){
                if (abs(delta_ud) > abs(delta_lr)){
                    gesture_found = 2;
                }
                else {
                    gesture_found = 3;
                }
            }

        }

        n_to_read = n_to_read - 1;  
        count += 1;

        //printf("read from buffer: %x\n", mot_buf);
        //printf("Up:%x, Down:%x, Left:%x, Right:%x\n", mot_buf[0], mot_buf[1], mot_buf[2], mot_buf[3]);

    }

    set_gmode(0);
    return gesture_found;


}

static inline uint32_t rgbc_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}


void printI2Cbus(){
 
    uint offset = pio_add_program(pio, &i2c_program);
    i2c_program_init(pio, sm, offset, PIN_SDA, PIN_SCL);
 
    printf("\nPIO I2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }
        // Perform a 0-byte read from the probe address. The read function
        // returns a negative result NAK'd any time other than the last data
        // byte. Skip over reserved addresses.
        int result;
        if (reserved_addr(addr))
            result = -1;
        else
            result = pio_i2c_read_blocking(pio, sm, addr, NULL, 0, false);

        printf(result < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("I2C scan done.\n\n");
    // Finished bus scan
   
    while(1){
        returned = apds_init();
        if(returned <0){
         printf("cannot initialize ADPS-9960. Retry...\n");
         }
         else{  // only after successfully initialize sensor, go to next step
            break;
         }
    }
    printf("APDS Initialization Done\n");
    
}


/*
int main() {
    int32_t r,g,b,c;
    uint32_t color;
    int32_t proximity;
    int gesture_found;
       

    stdio_init_all();

    while(stdio_usb_connected()!=true);
    sleep_ms(2000);

    uint offset = pio_add_program(pio, &i2c_program);
    i2c_program_init(pio, sm, offset, PIN_SDA, PIN_SCL);
 
    printf("\nPIO I2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }
        // Perform a 0-byte read from the probe address. The read function
        // returns a negative result NAK'd any time other than the last data
        // byte. Skip over reserved addresses.
        int result;
        if (reserved_addr(addr))
            result = -1;
        else
            result = pio_i2c_read_blocking(pio, sm, addr, NULL, 0, false);

        printf(result < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("I2C scan done.\n\n");
    // Finished bus scan
   
    while(1){
        returned = apds_init();
        if(returned <0){
         printf("cannot initialize ADPS-9960. Retry...\n");
         }
         else{  // only after successfully initialize sensor, go to next step
            break;
         }
    }
    printf("APDS Initialization Done\n");
    

    neopixel_init();
    while (returned >= 0) {

        read_proximity(&proximity);
        printf("distance:%d\n", proximity);
        
        // gesture_found = read_gesture();
        // printf("Gesture is: %d", gesture_found);
        // sleep_ms(100);

        //read_rgbc(&r, &g, &b, &c);
        //printf("r:%d, g:%d, b:%d, c:%d\n", r, g, b, c);

        //color = rgbc_u32((uint8_t)r, (uint8_t)g,(uint8_t)b);

        // gpio_put(WS2812_POWER_PIN, true);
        // gpio_put(WS2812_PIN, true);
        // neopixel_set_rgb(color);
        sleep_ms(2000);
        
    }

    return 0;
}
*/

//                  cd pico/pico-examples/build/motion
