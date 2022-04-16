/* Copyright (C) 2021 Simon Frauenschuh & Philip Jessner - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */
#include <sys/time.h>
#include "touchpanel.h"

#pragma once

// Logic for mode single-point (define a point, where the ball should move to)
void moveToPoint(int xEst, int yEst) {
    // Touchpanel-specific values for the center
	int xMid = 152, yMid = 114;
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
		dX = ((touchpanelPositionXOld - touchpanelPositionX) / microseconds); //* 730.0;
		dY = ((touchpanelPositionYOld - touchpanelPositionY) / microseconds); //* 730.0;
		// Start measuring time
    	gettimeofday(&begin, 0);

		pX = 407 - (double)(touchpanelPositionX - xMid); /// 80;
		pY = 395 - (double)(touchpanelPositionY - yMid); /// 120;

		pwmWrite(PIN_BASE + 1, pX + dX);
		//pwmWrite(PIN_BASE + 0, pY + dY);
		}
}