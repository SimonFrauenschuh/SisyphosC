/* Copyright (C) 2021 Simon Frauenschuh & Philip Jessner - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */

// Username: SimonFrauenschuh
// Password Token: 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

#include "pca9685.h"
#include "lib/servo.h"
#include "lib/gyroscope.h"
#include "lib/touchpanel.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Defined in servo.h
// int errorCode = 0;

// Reads out the current position from the Servo-Driver and the Gyroscope
// If the deviation of those two values is too high, an error code is sent
void getServoPosition(double* servoPositionX, double* servoPositionY);

// Logic for mode single-point (define a point, where the ball should move to)
void moveToPoint(int xEst, int yEst);

int main() {
	// Check if the user is root & connect to touchpanel
	checkUser();
	firstSetupTouchpanel();

	// Connect and calibrate Servos
	firstSetupServo();
	setServoNull();

	// Connect and calibrate Gyroscope
	firstSetupGyro();
	calibrateGyro();
	printf("====================\nCalibration finished\n====================\n\n");

	// Later: dependending on chosen mode (DB)
	while (1) 	{
		moveToPoint(7800, 7400);
	}

	printf("\n\nError Code: %d\n", errorCode);
	close(connectionTouchpanel);
	return 0;
	}

void moveToPoint(int xEst, int yEst) {
	// Touchpanel-specific values for the center
	int xMid = 7800, yMid = 7400;
	int touchpanelPositionX, touchpanelPositionY;
	int touchpanelPositionXOld, touchpanelPositionYOld;
	// Values for the PD-Regulator
	double dX, dY, pX, pY;

	// Used for the "D" Part of the PD-Controller
	struct timeval begin, end;
	long microseconds;

	// Start measuring time
    gettimeofday(&begin, 0);

	while (1) {	
		touchpanelPositionXOld = touchpanelPositionX;
		touchpanelPositionYOld = touchpanelPositionY;

		getTouchpanelPosition(&touchpanelPositionX, &touchpanelPositionY);
		printf("xPosition: %d    |    yPosition: %d\n", touchpanelPositionX, touchpanelPositionY);

		// Stop measuring time and calculate the elapsed time
    	gettimeofday(&end, 0);
    	microseconds = end.tv_usec - begin.tv_usec;
		// Calculate the "D" Part of the PD-Controller
		dX = (touchpanelPositionXOld - touchpanelPositionX) / (microseconds / 580.0);
		dY = - (touchpanelPositionYOld - touchpanelPositionY) / (microseconds / 530.0);
		// Start measuring time
    	gettimeofday(&begin, 0);

		pX = 408 - (double)(touchpanelPositionX - xMid) / 550;
		pY = 385 + (double)(touchpanelPositionY - yMid) / 980;

		pwmWrite(PIN_BASE + 1, pX + dX);
		pwmWrite(PIN_BASE + 0, pY + dY);
		}
	}

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
