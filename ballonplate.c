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
#include "lib/logic.h"
#include "lib/database.h"
#include "lib/resultThread.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Reads out the current position from the Servo-Driver and the Gyroscope
// If the deviation of those two values is too high, an error code is sent
void getServoPosition(double* servoPositionX, double* servoPositionY);

int main() {

	// Check if the user is root & connect to touchpanel (USB)
	//checkUser();
	//firstSetupTouchpanelUSB();
	firstSetupTouchpanelADC(0x48);

	// Connect and calibrate Servos
	firstSetupServo();
	setServoNull();

	// Connect and calibrate Gyroscope
	//firstSetupGyro();
	//calibrateGyro();
	printf("====================\nCalibration finished\n====================\n\n");

	// Initialize the db-connection
	createDBconnection();
	int mode, xEst, yEst;

	// Create a thread for updating the result in the db every second
	pthread_t tid;
	pthread_create(&tid, NULL, &threadproc, NULL);

	while (errorCode == 0) {
		mode = readDatabase("mode");
		xEst = readDatabase("xest");
		yEst = readDatabase("yest");

		if (mode == 1) {
			moveToAngle(xEst, yEst);
		} else {
			moveToPoint(179, 106);
		}
	}
	killDBconnection();
	printf("\n\nError Code: %d\n", errorCode);
	return 0;
}

// TODO #############################################################################################################
void getServoPosition(double* servoPositionX, double* servoPositionY) {
	// Gets the raw PWM Data from the Servo and decrypts the original angle
	*servoPositionY = readServoPosition(0);
	*servoPositionX = readServoPosition(1);
	*servoPositionX -= CHANNEL1_MID * 2;
	*servoPositionX *= 180;
	*servoPositionX -= 90;
	*servoPositionY -= CHANNEL0_MID * 2;
	*servoPositionY *= 180;
	*servoPositionY -= 89;
}