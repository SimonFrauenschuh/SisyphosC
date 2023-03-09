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

int touchpanelPositionX[3] = {0}, touchpanelPositionY[3] = {0};


inline void calculatePWMSignal(int xEst, int yEst, int milliseconds) {
	int xDiff, yDiff;
	int pX = 0, pY = 0;
	int dX = 0, dY = 0;
	
	printf("xPosition: %d	|	yPosition: %d\n", touchpanelPositionX[0], touchpanelPositionY[0]);

	xDiff = touchpanelPositionX[0] - xEst;
	// Add a offset and calculate the p-Part (Position, [pixels])
	//pX = (xDiff > 0) ? (5 + xDiff / 25) : (-5 + xDiff / 25);


	yDiff = touchpanelPositionY[0] - yEst;
	pY = (yDiff > 0) ? (4 + yDiff / 25) : (-4 + yDiff / 25);
	

	// Calculate the "D" Part of the PD-Controller (Speed, [pixels/ms])
	/*if ((touchpanelPositionX[0] < xEst + 60) && (touchpanelPositionX[0] > xEst -60)) {
		for (int i = 0; i < (sizeof(touchpanelPositionX) / sizeof(int)) - 1; i++) {
			xDiff = touchpanelPositionX[i + 1] - touchpanelPositionX[i];
			dX -= ((3.5 - 1.5 * i) * xDiff) / milliseconds;
		}
	}*/

	if ((touchpanelPositionY[0] < yEst + 50) && (touchpanelPositionY[0] > yEst - 50)) {
		for (int i = 0; i < (sizeof(touchpanelPositionY) / sizeof(int)) - 1; i++) {
			yDiff = touchpanelPositionY[i + 1] - touchpanelPositionY[i];
			dY -= ((4 - 1.2 * i) * yDiff) / milliseconds;
		}
	}


	if (pX + dX > 20) {
		pX = 10;
		dX = 10;
	} else if (pX + dX < -20) {
		pX = -10;
		dX = -10;
	}
	if (pY + dY > 20) {
		pY = 15;
		dY = 10;
	} else if (pY + dY < -20) {
		pY = -10;
		dY = -10;
	}

	pwmWrite(PIN_BASE + 0, CHANNEL0_MID - (pY + dY));
	pwmWrite(PIN_BASE + 1, CHANNEL1_MID + (pY + dY));
	pwmWrite(PIN_BASE + 2, CHANNEL2_MID + (pX + dX));
	pwmWrite(PIN_BASE + 3, CHANNEL3_MID - (pX + dX));

	printf("Y: %d, %d\n", pY, dY);
}

// Logic for mode single-point (define a point, where the ball should move to)
inline void moveToPoint(int xEst, int yEst) {
	// Used for the "D" Part of the PD-Controller
	struct timeval begin, end;
	long milliseconds;

	// Start measuring time
    gettimeofday(&begin, 0);

	// Delete the last value and shift the others
	for (int i = (sizeof(touchpanelPositionX) / sizeof(int)) - 1; i > 0 ; i--) {
		touchpanelPositionX[i] = touchpanelPositionX[i - 1];
	}
	for (int i = (sizeof(touchpanelPositionY) / sizeof(int)) - 1; i > 0 ; i--) {
		touchpanelPositionY[i] = touchpanelPositionY[i - 1];
	}
	
	getTouchpanelPositionADC(&touchpanelPositionX[0], &touchpanelPositionY[0]);

	// Stop measuring time and calculate the elapsed time
	gettimeofday(&end, 0);
	milliseconds = (end.tv_usec - begin.tv_usec) / 1000;
	// Start measuring time
	gettimeofday(&begin, 0);

	// Control, if there isn't a misread value from the touchpad
	// Sometimes, the touchpanel has a state, where "0" is sent for the x-Coordinate, no matter, what the real value is
	// Also ignore when it took too long (--> negative values that'd destroy the controllers algorithm)
	if (touchpanelPositionX[0] != 0 && touchpanelPositionX[0] != 505 && touchpanelPositionY[0] != 0 && milliseconds > 1) {
		
		// Register the new value in the db
		writeDatabaseXY(touchpanelPositionX[0], touchpanelPositionY[0]);
		
		int dev = 15;
		// Single values, that don't match, get ignored
		if ((!(((touchpanelPositionX[0] < touchpanelPositionX[1] - 20) || (touchpanelPositionX[0] > touchpanelPositionX[1] + 20) || (touchpanelPositionY[0] < touchpanelPositionY[1] - 20) || (touchpanelPositionY[0] > touchpanelPositionY[1] + 20))))
		// And if the trend is staying the same
		&& (((touchpanelPositionX[2] < touchpanelPositionX[1] + dev) && (touchpanelPositionX[1] < touchpanelPositionX[0] + dev))
		|| ((touchpanelPositionX[2] > touchpanelPositionX[1] - dev) && (touchpanelPositionX[1] > touchpanelPositionX[0] - dev)))
		&& (((touchpanelPositionY[2] < touchpanelPositionY[1] + dev) && (touchpanelPositionY[1] < touchpanelPositionY[0] + dev))
		|| ((touchpanelPositionY[2] > touchpanelPositionY[1] - dev) && (touchpanelPositionY[1] > touchpanelPositionY[0] - dev)))) {
			calculatePWMSignal(xEst, yEst, milliseconds);
		} // If the previous-previous value (--> milliseconds * 3 (a bit less important)) matches with the new one
		else if (!(((touchpanelPositionX[0] < touchpanelPositionX[2] - 40) || (touchpanelPositionX[0] > touchpanelPositionX[2] + 40) || (touchpanelPositionY[0] < touchpanelPositionY[2] - 40) || (touchpanelPositionY[0] > touchpanelPositionY[2] + 40)))) {
			// I don't know why, but this works way better...?
			touchpanelPositionX[0] = touchpanelPositionX[1];
			touchpanelPositionY[0] = touchpanelPositionY[1];
			calculatePWMSignal(xEst, yEst, milliseconds * 2);
		}
	}
}

// Synchronize with the angle, given by the database (the phones gyroscope)
inline void moveToAngle(int xEst, int yEst) {
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