# SisyphosC
**Contains the Code / Workflow for the logic behind the Sisyphos 2.0 Project. Written in C**

## Initialize first time
1. Activate I2C Protokoll (settings)
2. Execute ```sudo apt install wiringpi```
3. Install librarys: navigate to lib folder and execute ```sudo make install```

## Start the application
1. Navigate to ```SisyphosC```
- For USB-Touchpanel: Execute ```sudo ./sisyphos```
- For ADC-Touchpanel: Execute ```./sisyphos```

## Compile the program after changes
1. For changes in the main program
- The Program needs to be stored in ```/home/pi/Desktop/SisyphosC/lib/ADS1115.h``` (or change the makefile according to your path)
- Navigate to ```SisyphosC```
- Execute ```make && make clean```
2. For changes in the PCA9658 file
- Navigate to ```SisyphosC/lib```
- Execute ```make```

### Note!
**The Y-Axis is the one, controlled by the lower Servo (Channel 0 & 2). The X-Axis ist controlled by the higher Servo (Channel 1 & 3).
The Makefile doesn't recompile the librarys, if a ".o" file exists. New written librarys or files need to be included in the correct makefile (in most cases the one located under ```.../SisyphosC/```).**
