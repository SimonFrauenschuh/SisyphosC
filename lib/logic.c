/* Copyright (C) 2021 Simon Frauenschuh & Philip Jessner - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#include "touchpanel.h"
#include "servo.h"
#include "database.h"

#pragma once

// Logic for mode single-point (define a point, where the ball should move to)
void moveToPoint(int xEst, int yEst) {
	int xDiff, yDiff;
	int touchpanelPositionX, touchpanelPositionY;
	int touchpanelPositionXOld, touchpanelPositionYOld;
	int touchpanelPositionXOldOld, touchpanelPositionYOldOld;
	// Values for the PD-Regulator
	int dX = 0, dY = 0, pX = 0, pY = 0;

	// Used for the "D" Part of the PD-Controller
	struct timeval begin, end;
	long milliseconds;

	// Start measuring time
    gettimeofday(&begin, 0);

	touchpanelPositionXOldOld = touchpanelPositionXOld;
	touchpanelPositionYOldOld = touchpanelPositionYOld;
	touchpanelPositionXOld = touchpanelPositionX;
	touchpanelPositionYOld = touchpanelPositionY;

	getTouchpanelPositionADC(&touchpanelPositionX, &touchpanelPositionY);
	//getTouchpanelPositionUSB(&touchpanelPositionX, &touchpanelPositionY);
	writeDatabaseXY(touchpanelPositionX, touchpanelPositionY);
	printf("xPosition: %d    |    yPosition: %d\n", touchpanelPositionX, touchpanelPositionY);

	// Control, if there isn't a misread value from the touchpad
	// Single values, that don't match, get ignored
	if (!(((touchpanelPositionX < touchpanelPositionXOld * 0.7) && (touchpanelPositionX > touchpanelPositionXOld * 1.3)) || (((touchpanelPositionX < touchpanelPositionXOldOld * 0.7) && (touchpanelPositionX > touchpanelPositionXOldOld * 1.3))))) {
		// Stop measuring time and calculate the elapsed time
		gettimeofday(&end, 0);
		milliseconds = (end.tv_usec - begin.tv_usec) / 1000;
		// Start measuring time
		gettimeofday(&begin, 0);

		xDiff = touchpanelPositionX - xEst;
		if (xDiff < 0) {
			pX = -pow(xDiff / 3.2, 2) / 90;
		} else {
			pX = pow(xDiff / 3.2, 2) / 90;
		}
		yDiff = touchpanelPositionY - yEst;
		if (yDiff < 0) {
			pY = -pow(yDiff / 2.7, 2) / 40;
		} else {
			pY = pow(yDiff / 2.7, 2) / 40;
		}

		// Calculate the "D" Part of the PD-Controller
		if (xDiff > -50 && xDiff < 50) {
			dX = -10 * (touchpanelPositionXOld - touchpanelPositionX) / milliseconds;
		} else {
			dX = 0;
		}
		if (yDiff > -40 && yDiff < 40) {
			dY = -12 * (touchpanelPositionYOld - touchpanelPositionY) / milliseconds;
		} else {
			dY = 0;
		}
			
		if (pX + dX > 20) {
			pX = 10;
			dX = 10;
		} else if (pX + dX < -20) {
			pX = -10;
			dX = -10;
		}
		if (pY + dY > 20) {
			pY = 10;
			dY = 10;
		} else if (pY + dY < -20) {
			pY = -10;
			dY = -10;
		}

		pwmWrite(PIN_BASE + 0, CHANNEL0_MID - (pY + dY));
		pwmWrite(PIN_BASE + 1, CHANNEL1_MID + (pY + dY));
		pwmWrite(PIN_BASE + 2, CHANNEL2_MID + (pX + dX));
		pwmWrite(PIN_BASE + 3, CHANNEL3_MID - (pX + dX));

		printf("%d, %d\n\n", pX, pY);
	}
}

// Synchronize with the angle, given by the database (the phones gyroscope)
void moveToAngle(int xEst, int yEst) {
	xEst /= 2;
	yEst /= 2;
	if (xEst > 20) {
		xEst = 20;
	} else if (xEst < -20) {
		xEst = -20;
	}
	if (yEst > 20) {
		yEst = 20;
	} else if (yEst < -20) {
		yEst = -20;
	}
	setServoDegree(0, xEst);
	setServoDegree(1, xEst);
	setServoDegree(2, -yEst);
	setServoDegree(3, -yEst);
}