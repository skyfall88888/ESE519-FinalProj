# ESE519-FinalProj    “Wrangler”

## Demonstration

The Completed Wrangler looks like: 

![image](https://github.com/skyfall88888/ESE519-FinalProj/blob/main/Wrangler.jpg)

Gif in action:

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
| car chassis | 1  |
| servo motor | 1  |
| DC motor  | 2  |
| motor driver | 1 |
| joystick | 1 |
| sound sensor  | 1  |
| distance/motion sensor  | 1|
| voltage switch  | 1|

### From Proposal to Demo
This is our proposed model, cute and interesting. We were thinking using a motion sensor APDS9960 to control the horse to move to different directions. 

![image](https://user-images.githubusercontent.com/84453030/209241315-e3d71c71-575d-415a-b744-bcac83668311.png)

First thing we did was nailed the QY PY 2040 board. We read through the datasheet and made our "budget" for the pins and memories. The pins are assigned as below:

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
After we made all parts function together, we started mounting them on the chassis. We put the breadboard on the first layer of acylic board on the chassis, arrange the wires to made it look nicer, then we mounted a second layer of acrylic board to hold the "wrangler" and its whip.

<img width="329" alt="Screenshot 2022-12-22 at 6 27 30 PM" src="https://user-images.githubusercontent.com/84453030/209898140-7996e309-d591-415a-a19e-065560bb61e1.jpg"> <img width="329" alt="Screenshot 2022-12-22 at" src="https://user-images.githubusercontent.com/84453030/209898345-888ee383-af6d-402c-8cb5-d80cd5f6b820.jpg"> 

When we tested it out in the assembled version, we found that the wrangler stands too high and the whole thing would loss balance when it run, so we added some weights in the bottom layer to make it run smoother.

Finally it look like this!

<img width="229" alt="Screenshot 2022-12-22 at 6 27 30 PM" src="https://user-images.githubusercontent.com/84453030/209244690-b042743f-b8d5-4b7b-85bc-d5e7fa7b8b83.jpg"> <img width="229" alt="Screenshot 2022-30 PM" src="https://user-images.githubusercontent.com/84453030/209897828-662b3cfe-c631-4b5c-af62-9ba5810232ae.jpg"> <img width="229" alt="Screenshot 2022-12-2 PM" src="https://user-images.githubusercontent.com/84453030/209898459-58e787cb-3f0b-4564-a662-14d6d60ff8f2.jpg"> 

## Troubles we met
**1. Problem 1: Servo motor**

**Problem:** the starting, ending angle of servo motor are not precise. The rotation rate of servo motor is not stable.
Often times it just clogs.

**Solution:** implement an additional servo motor driver voltage level shifter breakout board that takes in a GPIO PWM and outputs a higher- voltage-protected PWM wave that feeds to the servo motor. With separate power supply and reworked PWM wave GPIO output, the behaviour is now stable.

**Problem 2: Sound sensor**
 
**Problem:** sound sensor tends to behave unstably with different main.c settings(loop VS one go). And it tends to have limited accuracy distinguishing between short sharp sounds and long gentle sounds.

**Solution:** Reconfigured the structure of code. Changing looped sound.c to a one-go structure. Redesigned the way function gets called. Redesigned the way ADC reads in the analog input and the threshold values.

![ezgif com-gif-maker (4)](https://user-images.githubusercontent.com/84453030/205533408-7cd24c92-dc22-4238-bcc6-a89e86ae4948.gif)

**Problem 3: joystick**

**Problem:** the output range of joystick is quite random. Distinguishing left and right can be confusing. Because of the analog nature of the joystick, there is no actual best solution. 
**Solution:** We redesigned the determining algorithm to better improve the accuracy. 

**Problem 4: main logic**
 
**Problem:** our initial plan was to pick up the following inputs in a sequencer: sound->apds->joystick. But when implementing this function, it gets stuck in one sensor, or, several inputs try to take over at the same time. This led to an extremely unstable response from the system.

Meanwhile, ADPS motion detection is unstable, it almost can only detect the presence of our hands but not the motion/direction. We reference the python src code for adps motion detection, and programmed out c code in similar algorithm logic. However, check debug output below, when hands waving on top of the sensor, whatever gesture we made, the raw data is present but not in big difference, after calculation, the gesture is most of the time treated as '0'(up).

<img width="324" alt="bug1 1" src="https://user-images.githubusercontent.com/84453030/205532401-33baf868-be4c-4dc0-b9ba-2ef550c36460.png">
<img width="324" alt="bug1 2" src="https://user-images.githubusercontent.com/84453030/205532409-d556929f-a6cf-4c06-a7e1-e7f32df8fe6f.png">

**Solution:** We decided to change the setting to a new algorithm: use apds to control/switch between 2 modes: 1. sound mode, 2. joystick mode. Waving the hand at the apds will switch one mode to another.
```mermaid
graph TD;
    A[Get distance data from ADPS 9960] --> B{< 200}
    B -->|Yes| C[Switch Mode]
    C --> D[Sound Mode] --> A
    C --> E[Joystick Mode] --> A
    B ---->|No| A
```

## What did we learn

## About PIO 
We used PIO for the LED on QT PY board and to drive the APDS9960 distance/motion sensor.

The general steps we programmed the PIO state machine are:

1. Determine which PIO instance to use(out of 2) PIO pio = pio0; PIO pio = pio1; 

2. Assign instructions into instruction memory with sufficient space uint offset = pio_add_program(pio, &program_name_here) 

3. Find an available state machine uint sm = pio_claim_unused_sm(pio, true); some_kind_of_program_init(pio, sm, offset, PICO_DEFAULT_LED_PIN); 
 
5. Up to this point, state machine is ready and running.



PIO's advantages:
1. With pio, we can basicly implement whatever protocol needed for the I/O communication. It's far more powerful and convenient over bit-banging. If using bit-banging, it will continuously consumes the memory and kernel of the PC, while the computer has other tasks to do.  for slower protocols you might be able to use an IRQ to wake up the processor from what it was doing fast enough (though latency here is a concern) to send the next bit(s). But PC-class processors keep many hundreds of instructions in-flight on a single core at once, which has drawbacks when trying to switch rapidly between hard real time tasks. 

2. PIO has it's strict timestamps, each instruction takes exactly one cycle, so it make it easier for the timing when programming and gives more control to the programmer. 

3. PIO states machine runs independently from the main processor, it saves memory, efficiency and pins.

These features make PIO a unique asset for a microcontroller.



## Our Team
Yu Feng   https://github.com/skyfall88888
Thea Yu   https://github.com/Thea-E
