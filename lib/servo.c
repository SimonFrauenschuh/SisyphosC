#include <wiringPi.h>
#include "pca9685.h"
#include <stdio.h>
#include <stdlib.h>

int connectionServo;
int errorCode = 0;

#pragma once

#define PIN_BASE 300
#define CHANNEL0_MIN 140
#define CHANNEL0_DIFF 510
// At home
#define CHANNEL1_MIN 145
// In school
//#define CHANNEL1_MIN 146
#define CHANNEL1_DIFF 523
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
		oldSet = (digitalRead(PIN_BASE + channel) & 0xFFF) + CHANNEL0_MIN / 1.0;
	} else {
		oldSet = (digitalRead(PIN_BASE + channel) & 0xFFF) + CHANNEL1_MIN  / 1.0;
	}
	return oldSet;
}

// Calculates individually for each Servo the needed PWM-Signal for the given angle
int calculateServoPWMSignal(int channel, double degree) {
	// Add 90 to "degree" to make function more user-friendly (range from -90 to 90 instead of 0 to 180)
	degree += 90.0;
	int move;
	// Logik to find out which channel to use (servo-specific parameters) and calculation of PWM-signal
	if (channel == 0) {
		if (degree == 0.0) {
			move = CHANNEL0_MIN;
		} else {
			move = (int)(CHANNEL0_DIFF * (degree / 180) + CHANNEL0_MIN);
		}
	} else {
		if (degree == 0.0) {
			move = CHANNEL1_MIN;
		} else {
			move = (int)(CHANNEL1_DIFF * (degree / 180) + CHANNEL1_MIN);
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
			diffSet = (move - oldSet) * 2.2;
		} else {
			diffSet = (move - oldSet) * 2;
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
	delay(500);
}
