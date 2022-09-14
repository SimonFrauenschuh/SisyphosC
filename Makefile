############################################################
# Copyright (C) 2021 Simon Frauenschuh - All Rights Reserved
# You may use and / or modify this code in
# terms of private use.
# Any caused damage or misbehaviour of any components are
# under the responsibility of the user and and the editor
# cannot be prosecuted for it
############################################################

# Note that the Makefile doesn't recompiles the librarys, if a .o file exists
# Cmd-statement "make && make clean"


# Including all the needed librarys
LDLIBS	= -lwiringPi -lwiringPiDev -lpthread -lm -lwiringPiPca9685
LDSERVO = -L/home/pi/Desktop/BallOnPlateC/lib/servo.h
LDGYRO = -L/home/pi/Desktop/BallOnPlateC/lib/gyroscope.h
LDTOUCH = -L/home/pi/Desktop/BallOnPlateC/lib/touchpanel.h
LDLOGIC = -L/home/pi/Desktop/BallOnPlateC/lib/logic.h
LDADC = -L/home/pi/Desktop/BallOnPlateC/lib/ADS1115.h

# Two-Step-Compiling
ballonplate:	ballonplate.o
	@gcc $(LDLIBS) $(LDSERVO) $(LDGYRO) $(LDTOUCH) $(LDLOGIC) $(LDADC) $< -o $@
	
ballonplate.o: ballonplate.c
	@gcc -c ballonplate.c

# Delete the ".o" files
.PHONY: clean

clean:
	@rm -f *.o ballonplate.o
	@echo [Compiled and successful]
