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

int main() {
	firstSetupTouchpanelADC(0x48);

	// Connect ato Servos
	firstSetupServo();
	setServoNull();

	// Initialize the db-connection
	createDBconnection();
	int mode, xEst, yEst;

	// Create a thread for updating the result in the db every second
	pthread_t tid;
	pthread_create(&tid, NULL, &threadproc, NULL);

	printf("====================\nInitialization finished\n====================\n\n");

	while (errorCode == 0) {
		mode = readDatabase("result", "mode");
		xEst = readDatabase("position", "xest");
		yEst = readDatabase("position", "yest");

		if ((mode == 1) || (mode == 2) || (mode == 3)) {
			moveToAngle(xEst, yEst);
		} else {
			moveToPoint(152, 114);
		}
	}
	killDBconnection();
	return 0;
}