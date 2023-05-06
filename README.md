# SisyphosC
**Contains the Code for the logic behind the Sisyphos 2.0 Project (Diploma Thesis).**

## Initialize first time
1. Activate I2C Protokoll (settings)
2. Execute ```sudo apt install wiringpi```
3. Install librarys: navigate to lib folder and execute ```sudo make install```

## Start the application
1. Navigate to ```SisyphosC```
2. Execute ```./sisyphos```

## Compile the program after changes
1. For changes in the main program
- Navigate to ```Sisyphos```
- Execute ```make && make clean```
2. For changes in the PCA9658 file
- Navigate to ```Sisyphos/lib```
- Execute ```make```

#### Arduino
The Arduino is used to control the led-strip. It could be controlled by the raspberry, but there was no time left to do this.

### Note!
**The Y-Axis is the one, controlled by the lower Servos (Channel 0 & 1). The X-Axis ist controlled by the higher Servos (Channel 2 & 3).
The Makefile doesn't recompile the librarys, if a ".o" file exists. New written librarys or files need to be included in the makefile (located under ```./Sisyphos/Makefile```).**
