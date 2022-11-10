#include <wiringPi.h>
#include "pca9685.h"
#include <stdio.h>
#include <stdlib.h>

int connectionServo;
int errorCode = 0;

#pragma once

#define PIN_BASE 300
// Servo bottom left
#define CHANNEL0_MID 520
// Servo bottom right
#define CHANNEL1_MID 462
// Servo top left
#define CHANNEL2_MID 362
// Servo top right
#define CHANNEL3_MID 395
#define HERTZ 60

// First initial setup (connection) for the Servos
void firstSetupServo() {
	printf("Connecting to Servos (I2C)...\n");
	// Calling wiringPi setup first (Lookup "Setup.txt")
	wiringPiSetup();

	// Setup with pinbase 300 and i2c location 0x41
	connectionServo = pca9685Setup(PIN_BASE, 0x41, HERTZ);
	if (connectionServo < 0) {
		errorCode = 1;
		fprintf(stderr, "---ERROR 1--- Connection Servo failure");
		exit(1);
	} else {
		// Reset all output, only for startup
		pca9685PWMReset(connectionServo);
	}
}

// Reads current position of the Servo
int readServoPosition(int channel) {
	int oldSet;
	// Add some extra value to increase wait duration for smaller movements (e.g. 40-->42)
	if (channel == 0) {
		oldSet = (digitalRead(PIN_BASE + channel) & 0xFFF) + CHANNEL0_MID;
	} else if (channel == 1) {
		oldSet = (digitalRead(PIN_BASE + channel) & 0xFFF) + CHANNEL1_MID;
	} else if (channel == 2) {
		oldSet = (digitalRead(PIN_BASE + channel) & 0xFFF) + CHANNEL2_MID;
	} else {
		oldSet = (digitalRead(PIN_BASE + channel) & 0xFFF) + CHANNEL3_MID;
	}
	return oldSet;
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
			move = (int)(CHANNEL0_MID + degree);
		}
	} else if (channel == 1) {
		if (degree == 0.0) {
			move = CHANNEL1_MID;
		} else {
			move = (int)(CHANNEL1_MID - degree);
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
	
		// Read the previous set from Controller to calculate later the duration for waiting, until the servo is in the right position
		int oldSet = readServoPosition(channel);
	
		// Move the Servo
		pwmWrite(PIN_BASE + channel, move);
	
		// Calculate the waiting duration
		int diffSet;
		if (channel == 0) {
			diffSet = (move - oldSet) * 1.0;
		} else if (channel == 1) {
			diffSet = (move - oldSet) * 1.0;
		} else if (channel == 2) {
			diffSet = (move - oldSet) * 1.0;
		} else {
			diffSet = (move - oldSet) * 1.0;
		}
		if (diffSet > 0) {
			delay(diffSet);
		} else {
			delay(-diffSet);
		}
	}
}

// Moves all Servos to 0 degree (horizontal)
void setServoNull() {
	printf("Setting Servos to Starting Point...\n");
	// Set Pin = 16 to set all Channels / Servos
	setServoDegree(16, 0);
	delay(200);
}
