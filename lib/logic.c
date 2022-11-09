/* Copyright (C) 2021 Simon Frauenschuh & Philip Jessner - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */
#include <sys/time.h>
#include <math.h>
#include "touchpanel.h"
#include "servo.h"

#pragma once

// Logic for mode single-point (define a point, where the ball should move to)
void moveToPoint(int xEst, int yEst) {
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

	while (1) {
		touchpanelPositionXOldOld = touchpanelPositionXOld;
		touchpanelPositionYOldOld = touchpanelPositionYOld;
		touchpanelPositionXOld = touchpanelPositionX;
		touchpanelPositionYOld = touchpanelPositionY;

		getTouchpanelPositionADC(&touchpanelPositionX, &touchpanelPositionY);
		//getTouchpanelPositionUSB(&touchpanelPositionX, &touchpanelPositionY);
		printf("xPosition: %d    |    yPosition: %d\n", touchpanelPositionX, touchpanelPositionY);

		// Control, if there isn't a misread value from the touchpad
		// Single values, that don't match, get ignored
		if ((!(touchpanelPositionX < touchpanelPositionXOld * 0.7) && !(touchpanelPositionX > touchpanelPositionXOld * 1.3)) || ((!(touchpanelPositionX < touchpanelPositionXOldOld * 0.7) && !(touchpanelPositionX > touchpanelPositionXOldOld * 1.3)))) {
			// Stop measuring time and calculate the elapsed time
			gettimeofday(&end, 0);
			milliseconds = (end.tv_usec - begin.tv_usec) / 1000;
			// Calculate the "D" Part of the PD-Controller
			dX = -6 * (touchpanelPositionXOld - touchpanelPositionX) / milliseconds;
			dY = -6 * (touchpanelPositionYOld - touchpanelPositionY) / milliseconds;
			// Start measuring time
			gettimeofday(&begin, 0);

			int xDiff = touchpanelPositionX - xEst;
			if (xDiff < 0) {
				pX = -pow(xDiff / 1.5, 2) / 220;
			} else {
				pX = pow(xDiff / 1.5, 2) / 220;
			}
			int yDiff = touchpanelPositionY - yEst;
			if (yDiff < 0) {
				pY = -pow(yDiff / 1.5, 2) / 180;
			} else {
				pY = pow(yDiff / 1.5, 2) / 180;
			}
			printf("%d, %d\n\n", pX, pY);
		}
		
		pwmWrite(PIN_BASE + 0, CHANNEL0_MID - (pY + dY));
		pwmWrite(PIN_BASE + 1, CHANNEL1_MID + (pY + dY));
		pwmWrite(PIN_BASE + 2, CHANNEL2_MID + (pX + dX));
		pwmWrite(PIN_BASE + 3, CHANNEL3_MID - (pX + dX));
	}
}