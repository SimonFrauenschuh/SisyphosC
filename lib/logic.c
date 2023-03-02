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


int touchpanelPositionX, touchpanelPositionY;
int touchpanelPositionXOld, touchpanelPositionYOld;
int touchpanelPositionXOldOld, touchpanelPositionYOldOld;
int dX, dY;


inline void calculatePWMSignal(int touchpanelPositionX, int touchpanelPositionY, int touchpanelPositionXOld, int touchpanelPositionYOld, int xEst, int yEst, int milliseconds) {
	int xDiff, yDiff;
	int pX = 0, pY = 0;
	
	printf("xPosition: %d	|	yPosition: %d\n", touchpanelPositionX, touchpanelPositionY);

	xDiff = touchpanelPositionX - xEst;
	// Add a offset and calculate the p-Part (Position, [pixels])
	//pX = (xDiff > 10) ? (4 + xDiff / 15) : ((xDiff < -10) ? (-4 + xDiff / 15) : (0));
	//pX = (xDiff > 0) ? (5 + xDiff / 25) : (-5 + xDiff / 25);


	yDiff = touchpanelPositionY - yEst;
	//pY = (yDiff > 0) ? (4 + yDiff / 25) : (-4 + yDiff / 25);
			
	// Calculate the "D" Part of the PD-Controller (Speed, [pixels/ms])
	if ((touchpanelPositionX < xEst + 50) && (touchpanelPositionX > xEst -50)) {
		xDiff = touchpanelPositionXOld - touchpanelPositionX;
		//dX = (xDiff > 0) ? (2 + 6 * xDiff / milliseconds) : (-2 + 6 * xDiff / milliseconds); 
		dX = (12 * xDiff) / milliseconds;
	} else {
		dX = 0;
	}

	yDiff = touchpanelPositionYOld - touchpanelPositionY;
	//dY = (yDiff > 0) ? (4 + 10 * yDiff / milliseconds) : (-4 + 10 * yDiff / milliseconds);
	//dY = (10 * yDiff) / milliseconds;


	if (pX + dX > 25) {
		pX = 15;
		dX = 10;
	} else if (pX + dX < -25) {
		pX = -15;
		dX = -10;
	}
	if (pY + dY > 25) {
		pY = 15;
		dY = 10;
	} else if (pY + dY < -25) {
		pY = -15;
		dY = -10;
	}

	pwmWrite(PIN_BASE + 0, CHANNEL0_MID - (pY + dY));
	pwmWrite(PIN_BASE + 1, CHANNEL1_MID + (pY + dY));
	pwmWrite(PIN_BASE + 2, CHANNEL2_MID + (pX + dX));
	pwmWrite(PIN_BASE + 3, CHANNEL3_MID - (pX + dX));

	printf("X: %d, %d\n", pX, dX);
}

// Logic for mode single-point (define a point, where the ball should move to)
inline void moveToPoint(int xEst, int yEst) {
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
	writeDatabaseXY(touchpanelPositionX, touchpanelPositionY);

	// Stop measuring time and calculate the elapsed time
	gettimeofday(&end, 0);
	milliseconds = (end.tv_usec - begin.tv_usec) / 1000;
	// Start measuring time
	gettimeofday(&begin, 0);

	// Control, if there isn't a misread value from the touchpad
	// Sometimes, the touchpanel has a state, where "0" is sent for the x-Coordinate, no matter, what the real value is
	// Also ignore when it took too long (--> negative values that'd destroy the controllers algorithm)
	if (touchpanelPositionX != 0 && touchpanelPositionY != 0 && milliseconds > 1) {
		int dev = 3;
		// Single values, that don't match, get ignored
		if ((!(((touchpanelPositionX < touchpanelPositionXOld - 20) || (touchpanelPositionX > touchpanelPositionXOld + 20) || (touchpanelPositionY < touchpanelPositionYOld - 20) || (touchpanelPositionY > touchpanelPositionYOld + 20))))
		// And if the trend is staying the same
		&& (((touchpanelPositionXOldOld < touchpanelPositionXOld + dev) && (touchpanelPositionXOld < touchpanelPositionX + dev))
		|| ((touchpanelPositionXOldOld > touchpanelPositionXOld - dev) && (touchpanelPositionXOld > touchpanelPositionX - dev)))
		&& (((touchpanelPositionYOldOld < touchpanelPositionYOld + dev) && (touchpanelPositionYOld < touchpanelPositionY + dev))
		|| ((touchpanelPositionYOldOld > touchpanelPositionYOld - dev) && (touchpanelPositionYOld > touchpanelPositionY - dev)))) {
			calculatePWMSignal(touchpanelPositionX, touchpanelPositionY, touchpanelPositionXOld, touchpanelPositionYOld, xEst, yEst, milliseconds);
		} // If the previous-previous value (--> milliseconds * 3 (a bit less important)) matches with the new one
		else if (!(((touchpanelPositionX < touchpanelPositionXOldOld - 40) || (touchpanelPositionX > touchpanelPositionXOldOld + 40) || (touchpanelPositionY < touchpanelPositionYOldOld - 40) || (touchpanelPositionY > touchpanelPositionYOldOld + 40)))) {
			calculatePWMSignal(touchpanelPositionX, touchpanelPositionY, touchpanelPositionXOldOld, touchpanelPositionYOldOld, xEst, yEst, milliseconds * 3);
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