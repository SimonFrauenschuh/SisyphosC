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

// Logic for mode single-point (define a point, where the ball should move to)
void moveToPoint(int xEst, int yEst) {
	int xDiff, yDiff;
	// Values for the PD-Regulator
	int dX = 0, dY = 0, pX = 0, pY = 0;

	// Used for the "D" Part of the PD-Controller
	struct timeval begin, end;
	long milliseconds;

	// Start measuring time
    gettimeofday(&begin, 0);

	touchpanelPositionXOld = touchpanelPositionX;
	touchpanelPositionYOld = touchpanelPositionY;

	getTouchpanelPositionADC(&touchpanelPositionX, &touchpanelPositionY);
	//getTouchpanelPositionUSB(&touchpanelPositionX, &touchpanelPositionY);
	//writeDatabaseXY(touchpanelPositionX, touchpanelPositionY);
	
	// Control, if there isn't a misread value from the touchpad
	// Sometimes, the touchpanel has a state, where "0" is sent for the x-Coordinate, no matter, what the real value is
	if (touchpanelPositionX != 0) {
		// Single values, that don't match, get ignored
		if (!((touchpanelPositionX < touchpanelPositionXOld * 0.7) || (touchpanelPositionX > touchpanelPositionXOld * 1.3) || (touchpanelPositionY < touchpanelPositionYOld * 0.7) || (touchpanelPositionY > touchpanelPositionYOld * 1.3))) {
			printf("xPosition: %d    |    yPosition: %d\n", touchpanelPositionX, touchpanelPositionY);
			
			// Stop measuring time and calculate the elapsed time
			gettimeofday(&end, 0);
			milliseconds = (end.tv_usec - begin.tv_usec) / 1000;
			// Start measuring time
			gettimeofday(&begin, 0);

			xDiff = touchpanelPositionX - xEst;
			// Add a offset and calculate the p-Part
			pX = (xDiff > 0) ? (pX = 8 + xDiff / 15) : (pX = -8 + xDiff / 15);
			
			yDiff = touchpanelPositionY - yEst;
			pY = (yDiff > 0) ? (pY = 8 + yDiff / 14) : (pY = -8 + yDiff / 14);
			
			// Calculate the "D" Part of the PD-Controller
			/*if (xDiff > 50 && xDiff < 50) {
				dX = -15 * (touchpanelPositionXOld - touchpanelPositionX) / milliseconds;
			}*/
			xDiff = touchpanelPositionXOld - touchpanelPositionX;
			dX = (xDiff < 0) ? (2 - 4 * xDiff / milliseconds) : (-2 - 4 * xDiff / milliseconds); 

			yDiff = touchpanelPositionYOld - touchpanelPositionY;
			dY = (yDiff < 0) ? (2 - 3 * yDiff / milliseconds) : (-2 - 3 * yDiff / milliseconds);
							
			if (pX + dX > 25) {
				pX = 15;
				dX = 10;
			} else if (pX + dX < -25) {
				pX = -15;
				dX = -10;
			}
			if (pY + dY > 20) {
				pY = 15;
				dY = 10;
			} else if (pY + dY < -20) {
				pY = -15;
				dY = -10;
			}

			pwmWrite(PIN_BASE + 0, CHANNEL0_MID - (pY + dY));
			pwmWrite(PIN_BASE + 1, CHANNEL1_MID + (pY + dY));
			pwmWrite(PIN_BASE + 2, CHANNEL2_MID + (pX + dX));
			pwmWrite(PIN_BASE + 3, CHANNEL3_MID - (pX + dX));

			printf("%d, %d\n\n", pX, dX);
		}
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