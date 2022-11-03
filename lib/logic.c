/* Copyright (C) 2021 Simon Frauenschuh & Philip Jessner - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */
#include <sys/time.h>
#include "touchpanel.h"
#include "servo.h"

#pragma once

// Touchpanel-specific values for the center
const int xMid = 152, yMid = 114;

// Logic for mode single-point (define a point, where the ball should move to)
void moveToPoint(int xEst, int yEst) {
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

		getTouchpanelPositionADC(&touchpanelPositionX, &touchpanelPositionY);
		//getTouchpanelPositionUSB(&touchpanelPositionX, &touchpanelPositionY);
		printf("xPosition: %d    |    yPosition: %d\n", touchpanelPositionX, touchpanelPositionY);

		// Stop measuring time and calculate the elapsed time
    	gettimeofday(&end, 0);
    	microseconds = end.tv_usec - begin.tv_usec;
		// Calculate the "D" Part of the PD-Controller
		dX = ((touchpanelPositionXOld - touchpanelPositionX) / microseconds) * 1.0;
		dY = -((touchpanelPositionYOld - touchpanelPositionY) / microseconds) * 1.0;
		// Start measuring time
    	gettimeofday(&begin, 0);

		pX = (touchpanelPositionX - xMid) / 6.0;
		pY = (touchpanelPositionY - yMid) / 40.0;
		
		pwmWrite(PIN_BASE + 0, CHANNEL0_MID + (int)(pY + dY));
		pwmWrite(PIN_BASE + 1, CHANNEL1_MID - (int)(pY + dY));
		pwmWrite(PIN_BASE + 2, CHANNEL2_MID + (int)(pX + dX));
		pwmWrite(PIN_BASE + 3, CHANNEL3_MID - (int)(pX + dX));
	}
}