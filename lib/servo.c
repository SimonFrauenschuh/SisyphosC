/* Copyright (C) 2021 Simon Frauenschuh & Sebastian Haider - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */

#include <wiringPi.h>
#include "pca9685.h"
#include <stdio.h>
#include <stdlib.h>

int connectionServo;
int errorCode = 0;

#pragma once

#define PIN_BASE 300
#define HERTZ 60
// Configuration Simon
// Servo bottom left
/*#define CHANNEL0_MID 520
// Servo bottom right
#define CHANNEL1_MID 468
// Servo top left
#define CHANNEL2_MID 363
// Servo top right
#define CHANNEL3_MID 400
*/
// Configuration Sebastian
// Servo bottom left
/*#define CHANNEL0_MID 500
// Servo bottom right
#define CHANNEL1_MID 440
// Servo top left
#define CHANNEL2_MID 345
// Servo top right
#define CHANNEL3_MID 425
*/
// Configuration Arduino Uno
// Servo bottom left
/*#define CHANNEL0_MID 500
// Servo bottom right
#define CHANNEL1_MID 448
// Servo top left
#define CHANNEL2_MID 363
// Servo top right
#define CHANNEL3_MID 380
*/
// Configuration Arduino Nano
// Servo bottom left
#define CHANNEL0_MID 430
// Servo bottom right
#define CHANNEL1_MID 478
// Servo top left
#define CHANNEL2_MID 322
// Servo top right
#define CHANNEL3_MID 353

// First initial setup (connection) for the Servos
void firstSetupServo() {
	printf("Connecting to Servos (I2C)...\n");
	// Calling wiringPi setup first (Lookup "Setup.txt")
	wiringPiSetup();

	// Setup with pinbase 300 and i2c location 0x41
	// 0x41 setup Simon; 0x40 setup Sebastian
	connectionServo = pca9685Setup(PIN_BASE, 0x40, HERTZ);
	if (connectionServo < 0) {
		errorCode = 1;
		fprintf(stderr, "---ERROR 1--- Connection Servo failure");
		exit(1);
	} else {
		// Reset all output, only for startup
		pca9685PWMReset(connectionServo);
	}
}

// Calculates individually for each Servo the needed PWM-Signal for the given angle
int calculateServoPWMSignal(int channel, double degree) {
	degree *= 2.4;
	int move;
	// Logik to find out which channel to use (servo-specific parameters) and calculation of PWM-signal
	if (channel == 0) {
		if (degree == 0.0) {
			move = CHANNEL0_MID;
		} else {
			move = (int)(CHANNEL0_MID - degree);
		}
	} else if (channel == 1) {
		if (degree == 0.0) {
			move = CHANNEL1_MID;
		} else {
			move = (int)(CHANNEL1_MID + degree);
		}
	} else if (channel == 2) {
		if (degree == 0.0) {
			move = CHANNEL2_MID;
		} else {
			move = (int)(CHANNEL2_MID + degree);
		}
	} else {
		if (degree == 0.0) {
			move = CHANNEL3_MID;
		} else {
			move = (int)(CHANNEL3_MID - degree);
		}
	}
	return move;
}

// Method to set a specific servo to a specific angle; without any Smoothing
void setServoDegree(int channel, double degree) {
	// If Channel 16 (all Servos) is choosen
	if (channel == 16) {
		setServoDegree(0, degree);
		setServoDegree(1, degree);
		setServoDegree(2, degree);
		setServoDegree(3, degree);
	} else {
		// Calculate the PWM-Signal
		int move = calculateServoPWMSignal(channel, degree);
	
		// Move the Servo
		pwmWrite(PIN_BASE + channel, move);
	}
}

// Moves all Servos to 0 degree (horizontal)
void setServoNull() {
	printf("Setting Servos to Starting Point...\n");
	// Set Pin = 16 to set all Channels / Servos
	setServoDegree(16, 0);
	delay(200);
}
