##########################################################################
# Makefile examples
#
# This software is a devLib extension to wiringPi <http://wiringpi.com/>
# and enables it to control the Adafruit PCA9685 16-Channel 12-bit
# PWM/Servo Driver <http://www.adafruit.com/products/815> via I2C interface.
#
# Copyright (c) 2014 Reinhard Sprung
#
# If you have questions or improvements email me at
# reinhard.sprung[at]gmail.com
#
# This software is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# The given code is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You can view the contents of the licence at <http://www.gnu.org/licenses/>.
##########################################################################

# Note that the Makefile doesn't recompiles the librarys, if a .o file exists

INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LDLIBS	= -lwiringPi -lwiringPiDev -lpthread -lm -lwiringPiPca9685
LDSERVO = -L/home/pi/Desktop/BallOnPlateC/lib/servo.h
LDGYRO = -L/home/pi/Desktop/BallOnPlateC/lib/gyroscope.h

# Should not alter anything below this line
###############################################################################

ballonplate:	ballonplate.o
	@gcc $(LDFLAGS) $(LDLIBS) $(LDSERVO) $(LDGYRO) $< -o $@
	
ballonplate.o: ballonplate.c
	@echo [link]
	@gcc -c ballonplate.c

.PHONY: clean

clean:
	rm -f *.o ballonplate.o
