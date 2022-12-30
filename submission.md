# ESE519-FinalProj    “Wrangler”

## Demonstration

The Completed Wrangler looks like: 

![image](https://github.com/skyfall88888/ESE519-FinalProj/blob/main/Wrangler.jpg)

### Functionality Description:

Two parts of the wrangler can be controlled over various ways: the whip and the movement of the Wrangler. There are two modes designed to control the Wrangler: through sound or joystick. 

Sound mode: make a sound towards to sound sensor triggers the whip. The whip gently hits the horse once and the Wrangler moves forward.

Joystick mode: Using the joystick to control the Wrangler to move in 3 directions: forward, left or right. 

### Gif in action:

---Joystick Mode:

Use the joystick located on the left hand side of the horse to control the direction of the Wrangler.

https://github.com/skyfall88888/ESE519-FinalProj/blob/main/Wrangler_joystick_mode.gif

---Sound Mode:

Make any sound would drive the Wrangler forward, at the same time, wave the whip and hit the horse once.

https://github.com/Thea-E/ESE519-FinalProj/blob/main/Wrangler_sound_mode.gif

## How we made it

System diagram:

<img width="500" alt="image" src="https://user-images.githubusercontent.com/84453030/209240112-c423f75a-0717-40b7-984f-ed91b1347722.png">

Components list:

| Component Name| Quantity |
| ------------- | ------------- |
| Car Chassis | 1  |
| Servo Motor | 1  |
| DC Motor  | 2  |
| Motor Driver | 1 |
| Joystick | 1 |
| Sound Sensor  | 1  |
| Distance/motion Sensor  | 1|
| Voltage Switch  | 1|

### Development
Attached below is the proposed idea of Wrangler(which is a bit different from the final deliverable). The proposed model is cute and interesting. We were thinking using a motion sensor APDS9960 to control the Wrangler to move in different directions. 

![image](https://user-images.githubusercontent.com/84453030/209241315-e3d71c71-575d-415a-b744-bcac83668311.png)

First thing we did was to plan the use of the pins of the QT PY 2040 board. We read through the datasheet and allocated the pins and memories for different uses. The pins are assigned as below:

<img width="429" alt="Screenshot 2022-12-22 at 6 27 30 PM" src="https://user-images.githubusercontent.com/84453030/209897570-e8ad2b4f-622c-4f0a-a5f1-6b90ff40baee.jpeg"> <img width="429" alt="Screenshot 2022-12-22 at 6 27 30 PM" src="https://user-images.githubusercontent.com/84453030/209241916-03a30a64-1e45-40bc-acb7-b7f7aba44143.png">

We started from making each small module work and then combined them altogether. They can be summarized as 
```mermaid
graph LR;
    QT_PY-->motion_sensor-->motor_driver-->2_DC_motors;   
```
```mermaid
graph LR;
    QT_PY --> joystick --> motor_driver --> 2_DC_motors ;
```
```mermaid
graph LR;
    QT_PY --> sound_sensor --> servo_motor & motor_driver;
```
After coding, configuring, testing all the broke-down functions mentioned, we mounted them onto the chassis. The breadboard was secured on the first layer of acylic board on the chassis, the wires were carefully arranged for clarity and ease of debugging. The second layer of acrylic board was used hold the "wrangler" toy for a better visual prospective.

<img width="329" alt="Screenshot 2022-12-22 at 6 27 30 PM" src="https://user-images.githubusercontent.com/84453030/209898140-7996e309-d591-415a-a19e-065560bb61e1.jpg"> <img width="329" alt="Screenshot 2022-12-22 at" src="https://user-images.githubusercontent.com/84453030/209898345-888ee383-af6d-402c-8cb5-d80cd5f6b820.jpg"> 

Then we tried to test the functions in an integrated manner by redesigning the C file, Cmake files and code structures. After failing due to a few bugs, we managed to get functions working. However, there are still structural problems that we found out during the process. The wrangler was placed too high and when the motors drive the chassis to move, especially forward,  the Wrangler loss balance. Thus, we decided to implement some weights at the bottom layer to lower the center of gravity and adjusting the running interval of motor for a smoother movement. 

The final deliverable looks like this: !


<img width="229" alt="Screenshot 2022-12-22 at 6 27 30 PM" src="https://user-images.githubusercontent.com/84453030/209244690-b042743f-b8d5-4b7b-85bc-d5e7fa7b8b83.jpg"> <img width="229" alt="Screenshot 2022-30 PM" src="https://user-images.githubusercontent.com/84453030/209897828-662b3cfe-c631-4b5c-af62-9ba5810232ae.jpg"> <img width="229" alt="Screenshot 2022-12-2 PM" src="https://user-images.githubusercontent.com/84453030/209898459-58e787cb-3f0b-4564-a662-14d6d60ff8f2.jpg"> 

## Troubles we met
**1. Problem 1: Servo motor**

**Problem:** the starting, ending angle of servo motor are not precise. The rotation rate of servo motor is not stable. It frequently clogs when not placed horizontally.

**Solution:** implement an additional servo motor driver voltage level shifter breakout board that takes in a GPIO PWM and outputs a higher- voltage-protected PWM wave that feeds to the servo motor. With separate power supply and reworked PWM wave GPIO output, the behaviour is now stable. It solves the angle precision problem and the clogging problem at the same time with the additional breakout board.

**Problem 2: Sound sensor**
 
**Problem:** sound sensor tends to behave unstably with different main.c settings(loop VS one go). And it tends to have limited accuracy distinguishing between short sharp sounds and long gentle sounds.

**Solution:** Reconfigured the structure of code. Changing looped sound.c to a one-go structure. Redesigned the way function gets called. Redesigned the way ADC reads in the analog input and the threshold values.

![ezgif com-gif-maker (4)](https://user-images.githubusercontent.com/84453030/205533408-7cd24c92-dc22-4238-bcc6-a89e86ae4948.gif)

**Problem 3: joystick**

**Problem:** the analog output range of joystick is quite random and with some delay. Such behavior is not observable through oscillioscope. The output value of a left push and a right push are distinct for every trail and dependent on previous trail if two pushes were conducted in a short period of time.  Because we are using ADC function of QT PY to read in analog ADC count, and the analog output nature of the joystick, the only way to improve the accuracy is algirithmic.. 

**Solution:** We redesigned the determining algorithm to better improve the accuracy and implemented updated timing contraints to the reading function to minimize the effect between each push.

**Problem 4: main logic**
 
**Problem:** our initial plan was to pick up the following inputs in a sequencer: sound->apds->joystick. But when implementing this function, we were struggling in deciding if we should let the Wrangler wait for one sensor to get input then move on, or wait a certain interval of time for input then move on. In either case, a lagging situation is definitely going to happen if user inputs through one sensor when system is currently waiting for another sensor to read inputs. It can also happen that several inputs are made at the same time but only one of them should actually "instruct" the wrangler to behave. This led to an extremely unstable response from the system.

**Solution:** We decided to change the setting to a new algorithm: use APDS sensor(proxomity) to control/switch between 2 modes: 1. sound mode, 2. joystick mode. Waving the hand at the apds will switch one mode to another.
```mermaid
graph TD;
    A[Get distance data from ADPS 9960] --> B{< 200}
    B -->|Yes| C[Switch Mode]
    C --> D[Sound Mode] --> A
    C --> E[Joystick Mode] --> A
    B ---->|No| A
```

**Problem 5: APDS Motion Detection**

We attempted ADPS motion detection extensively through adopting the detection algorithm developed in the source code provided(in Python). The behavior, however, is extremely unstable. It certainly is able to detect the presence of our hands but not necessarily the motion/direction. We also adopted a complicated determining algorithm to find the actual motion. But the first step-reading in values, is rather unstable. An example of such error is illustrated below:  when hands waving on top of the sensor, whatever gesture we made, the raw data is present but not in big difference, after calculation, the gesture is most of the time treated as '0'(up).

<img width="324" alt="bug1 1" src="https://user-images.githubusercontent.com/84453030/205532401-33baf868-be4c-4dc0-b9ba-2ef550c36460.png">
<img width="324" alt="bug1 2" src="https://user-images.githubusercontent.com/84453030/205532409-d556929f-a6cf-4c06-a7e1-e7f32df8fe6f.png">

## What did we learn

1. Method of dealing with multi-file C project based on embedded systems. 

2. Use of Cmake tool to configure and compile files involving multiple libraries.

3. Real-world embedded system debugging including both hardware and software sections.

4. Implement hardware-based circuits with software control.


## About PIO 
PIO was used for the LED(mode indicator) and to drive the APDS9960 distance/motion sensor.

The general steps we programmed the PIO state machine are:

1. Determine which PIO instance to use(out of 2) PIO pio = pio0; PIO pio = pio1; 

2. Assign instructions into instruction memory with sufficient space uint offset = pio_add_program(pio, &program_name_here) 

3. Find an available state machine uint sm = pio_claim_unused_sm(pio, true); some_kind_of_program_init(pio, sm, offset, PICO_DEFAULT_LED_PIN); 
 
5. Up to this point, state machine is ready and running.



PIO's advantages:
1. With pio, we can effectively implement any protocol at our need for the I/O communication. It's far more powerful and convenient over bit-banging. If using bit-banging, it will continuously consumes the memory and kernel of the PC, while the computer has other tasks to do.  For slower protocols you might be able to use an IRQ to wake up the processor from what it was doing fast enough (though latency here is a concern) to send the next bit(s). But PC-class processors keep many hundreds of instructions in-flight on a single core at once, which has drawbacks when trying to switch rapidly between hard real time tasks. 

2. PIO has it's strict timestamps, each instruction takes exactly one cycle, so it make it easier for the timing when programming and gives more control to the programmer. 

3. PIO states machine runs independently from the main processor, it saves memory, efficiency and pins.

These features make PIO a unique asset for a microcontroller.

## Code for Wrangler

The code files for the Wrangler can be found at the following link:

https://github.com/skyfall88888/ESE519-FinalProj/blob/main/Wrangler.zip




## Our Team
Yu Feng   https://github.com/skyfall88888
Thea Yu   https://github.com/Thea-E
