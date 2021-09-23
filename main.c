/* Copyright (C) 2021 Simon Frauenschuh - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */
 
// Note that the Makefile doesn't recompiles the librarys, if a .o file exists
 
// Username: SimonFrauenschuh
// Password Token: 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

#include "pca9685.h"
#include "lib/servo.h"
#include "lib/gyroscope.h"

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>

// Defined in servo.h
// int errorCode = 0;

// Reads out the current position from the Servo-Driver and the Gyroscope
// If the deviation of those two values is too high, an error code is sent
void getServoPosition(int *servoPositionX, int *servoPositionY) {
	// Gets the raw PWM Data from the Servo and decrypts the original angle
	*servoPositionY = readServoPosition(0);
	*servoPositionX = readServoPosition(1);
	*servoPositionX -= CHANNEL1_MIN * 2;
	*servoPositionX *= 180;
	*servoPositionX /= CHANNEL1_DIFF;
	*servoPositionX -= 90;
	*servoPositionY -= CHANNEL0_MIN * 2;
	*servoPositionY *= 180;
	*servoPositionY /= CHANNEL0_DIFF;
	*servoPositionY -= 89;
	
	
	// Correct the values, measured by the Gyroscope
	getGyroDegree();
	correctAngle();
	
	// Control, if the two values match nearly, else write a error code to the DB
	int deviation = 30;
	if ((gyroscopeXReal > (*servoPositionX + deviation)) || (gyroscopeXReal < (*servoPositionX - deviation)) || (gyroscopeYReal > (*servoPositionY + deviation)) || (gyroscopeYReal < (*servoPositionY - deviation))) {
		errorCode = 3;
		exit(3);
	}
}

int main(int argc, char **argv) {
	// Connect and calibrate Servos
	firstSetupServo();
	setServoNull();
	
	// Connect and calibrate Gyroscope
	firstSetupGyro();
	calibrateGyro();
	printf("====================\nCalibration finished\n====================\n\n");
	
	int servoPositionX = 0;
	int servoPositionY = 0;
	
	setServoDegreeSmooth(0, 0);
	setServoDegree(0, 20);
	getServoPosition(&servoPositionX, &servoPositionY);
	printf("xDrehung1: %d\nyDrehung1: %d\n", servoPositionX, servoPositionY);
	setServoDegree(0, -20);
	getServoPosition(&servoPositionX, &servoPositionY);
	printf("xDrehung1: %d\nyDrehung1: %d\n", servoPositionX, servoPositionY);
	setServoDegree(0, 0);
	getServoPosition(&servoPositionX, &servoPositionY);
	printf("xDrehung1: %d\nyDrehung1: %d\n", servoPositionX, servoPositionY);
	
	printf("\n\nError Code: %d\n", errorCode);
	return 0;
}

