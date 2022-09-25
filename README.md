# BallOnPlateC
**Contains the Code / Workflow for the logic behind the BallOnPlate Project. Written in C**

## Initialize first time
1. Activate I2C Protokoll (settings)
2. Execute ```sudo apt install wiringpi```
3. Install librarys: navigate to lib folder and execute ```sudo make install```

## Start the application
1. Navigate to ```BallOnPlateC```
- For USB-Touchpanel: Execute ```sudo ./ballonplate```
- For ADC-Touchpanel: Execute ```./ballonplate```

## Compile the program after changes
1. For changes in the main program
- Navigate to ```BallOnPlateC```
- Execute ```make && make clean```
2. For changes in the PCA9658 file
- Navigate to ```BallOnPlateC/lib```
- Execute ```make```

### Note!
**The Y-Axis is the one, controlled by the lower Servo (Channel 0 & 2). The X-Axis ist controlled by the higher Servo (Channel 1 & 3).
The Makefile doesn't recompile the librarys, if a ".o" file exists. New written librarys or files need to be included in the correct makefile (in most cases the one located under ```.../BallOnPlateC/```).**
