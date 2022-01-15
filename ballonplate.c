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

// Defined in servo.h
// int errorCode = 0;

// Reads out the current position from the Servo-Driver and the Gyroscope
// If the deviation of those two values is too high, an error code is sent
void getServoPosition(double *servoPositionX, double *servoPositionY);

// Logic for mode single-point (define a point, where the ball should move to)
void moveToPoint(int xEst, int yEst);

int main()
{
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
	while (1)
	{
		moveToPoint(7800, 8000);
	}

	//setServoDegree(1, -10);

	printf("\n\nError Code: %d\n", errorCode);
	close(connectionTouchpanel);
	return 0;
}

void moveToPoint(int xEst, int yEst)
{
	int touchpanelPositionX;
	int touchpanelPositionY;
	int touchpanelOldX;
	int touchpanelOldY;
	getTouchpanelPosition(&touchpanelPositionX, &touchpanelPositionY);
	printf("xPosition: %d    |    yPosition: %d\n", touchpanelPositionX, touchpanelPositionY);

	double servoPositionX;
	double servoPositionY;
	int delay_ms = 0;
	while ((touchpanelPositionX < (xEst * 0.99)) || (touchpanelPositionX > (xEst * 1.01)))
	{
		getTouchpanelPosition(&touchpanelPositionX, &touchpanelPositionY);
		printf("xPosition: %d    |    yPosition: %d\n", touchpanelPositionX, touchpanelPositionY);

		if (touchpanelPositionX < touchpanelOldX) {
			if (touchpanelPositionX < (xEst * 0.8)) {
				servoPositionX = 3.0;
			} else if (touchpanelPositionX < (xEst * 0.92)) {
				servoPositionX = 2.5;
			} else if (touchpanelPositionX < (xEst * 0.98)) {
				servoPositionX = 2.0;
			}
		} else if (touchpanelPositionX > touchpanelOldX) {
			if (touchpanelPositionX > (xEst * 1.2)) {
				servoPositionX = -3.0;
			} else if (touchpanelPositionX > (xEst * 1.08)){
				servoPositionX = -2.5;
			} else if (touchpanelPositionX > (xEst * 1.02)) {
				servoPositionX = -2.0;
			}
		}
		setServoDegree(1, servoPositionX);
		delay(delay_ms);
		touchpanelOldX = touchpanelPositionX;
	}
}

void getServoPosition(double *servoPositionX, double *servoPositionY)
{
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
	if ((gyroscopeXReal > (*servoPositionX + deviation)) || (gyroscopeXReal < (*servoPositionX - deviation)) || (gyroscopeYReal > (*servoPositionY + deviation)) || (gyroscopeYReal < (*servoPositionY - deviation)))
	{
		printf("---ERROR 3--- Gyroscope does not match estimated\n");
		errorCode = 3;
		exit(3);
	}
}
