# ESE519-FinalProj    “Wrangler”

### Overview

Updated system diagram:
<img width="480" alt="image" src="https://user-images.githubusercontent.com/84453030/205515665-1b245126-50fc-484e-a4bd-42989eca7e51.png">

Compared with original proposal, we are on track for all our functionality modules, while only changing the power source from battery to power bank, to better keep the whole system in the chassis and feed them with 5V voltage.

### Codes

Completed - DCmotor.c  servoMotor.c
In progress - adps.c  main.c
Reference - https://github.com/adafruit/Adafruit_CircuitPython_APDS9960/blob/main/adafruit_apds9960/apds9960.py  This is adps's sample code in python.

### Media
#### Materials
Our running vehicle (waiting for the wrangler to arrive):

![image](https://user-images.githubusercontent.com/84453030/205521412-5d8a650b-679e-4b45-9259-452804a8be71.png)
![image](https://user-images.githubusercontent.com/84453030/205521429-af146eba-2d22-4e09-9292-457b8813650b.png)

The sound control part:

![image](https://user-images.githubusercontent.com/84453030/205521457-d6b03502-353c-45a2-b24c-7d6a1ee23b23.png)

Might be considering to change to this big guy if later on the components cannot fit in:
![image](https://user-images.githubusercontent.com/84453030/205521501-02a5629b-883b-4684-a2f9-7f3358335de6.png)


#### Troubleshooting
ADPS motion detection is unstable, it almost can only detect the presence of our hands but not the motion/direction. We used distance detection to test the feasibility of our overall system logic, but wait to be decided whether to optimize motion algorithm, or change sensor, or change to picoML to do other fancy sensing.

ADPS minicom output:
wait to take screenshot in lab

#### Demos
![ezgif com-gif-maker (3)](https://user-images.githubusercontent.com/84453030/205521702-16c4c48e-d226-4052-a851-7adf0c6983d1.gif)
![ezgif com-gif-maker (2)](https://user-images.githubusercontent.com/84453030/205521640-61b15722-bcd2-42e4-85cf-e9bf814d1851.gif)


 
