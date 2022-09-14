/* Copyright (C) 2021 Simon Frauenschuh & Philip Jessner - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */

// Username: SimonFrauenschuh
// Password Token: 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

#include "lib/servo.h"
#include "lib/gyroscope.h"
#include "lib/logic.h"

#include <stdio.h>
#include <stdlib.h>

// Defined in servo.h
// int errorCode = 0;

// Reads out the current position from the Servo-Driver and the Gyroscope
// If the deviation of those two values is too high, an error code is sent
void getServoPosition(double* servoPositionX, double* servoPositionY);

int main() {

	// Check if the user is root & connect to touchpanel (USB)
	checkUser();
	//firstSetupTouchpanelUSB();
	firstSetupTouchpanelADC(0x48);

	// Connect and calibrate Servos
	firstSetupServo();
	setServoNull();

	// Connect and calibrate Gyroscope
	//firstSetupGyro();
	//calibrateGyro();
	printf("====================\nCalibration finished\n====================\n\n");

	// Later: dependending on chosen mode (DB)
	while (1) 	{
		moveToPoint(152, 114);
	}

	printf("\n\nError Code: %d\n", errorCode);
	return 0;
}

// TODO #############################################################################################################
void getServoPosition(double* servoPositionX, double* servoPositionY) {
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
	int deviation = 1000;
	if ((gyroscopeXReal > (*servoPositionX + deviation)) || (gyroscopeXReal < (*servoPositionX - deviation)) || (gyroscopeYReal > (*servoPositionY + deviation)) || (gyroscopeYReal < (*servoPositionY - deviation))) 	{
		printf("---ERROR 3--- Gyroscope does not match estimated\n");
		errorCode = 3;
		exit(3);
	}
}