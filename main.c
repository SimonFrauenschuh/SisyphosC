// Version 0.1
// Copyright @Simon Frauenschuh
// Username: SimonFrauenschuh
// Password Token: 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

#include "pca9685.h"

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PIN_BASE 300
// 577 - CHANNEL0_MIN
#define CHANNEL0_MIN 140
#define CHANNEL0_DIFF 437
// 584 - CHANNEL1_MIN
#define CHANNEL1_MIN 102
#define CHANNEL1_DIFF 482
#define HERTZ 60

int connectionGyroscope, connectionServo;

double gyroscopeXOffset = 0, gyroscopeYOffset = 0;
double gyroscopeXReal, gyroscopeYReal;
int correctionGyroscopeX = 0, correctionGyroscopeY = 0;

int errorCode = 0;

// First initial setup (connection) for the Servos
void firstSetupServo() {
	printf("Connecting to Servos (I2C)...\n");
	// Calling wiringPi setup first (Lookup "Setup.txt")
	wiringPiSetup();

	// Setup with pinbase 300 and i2c location 0x41
	connectionServo = pca9685Setup(PIN_BASE, 0x41, HERTZ);
	if (connectionServo < 0) {
		printf("Error in Servo Setup\n");
		errorCode = 1;
		printf("\n\nError Code: %d\n", errorCode);
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
int calculateServoPWMSignal(int channel, int degree) {
	// Add 90 to "degree" to make function more user-friendly (range from -90 to 90 instead of 0 to 180)
	degree += 90;
	int move;
	// Simple logik to find out which channel to use (servo-specific parameters) and calculation of PWM-signal
	if (channel == 0) {
		if (degree == 0) {
			move = CHANNEL0_MIN;
		} else {
			move = (int)(CHANNEL0_DIFF * ((float)degree / 180) + CHANNEL0_MIN);
		}
	} else {
		if (degree == 0) {
			move = CHANNEL1_MIN;
		} else {
			move = (int)(CHANNEL1_DIFF * ((float)degree / 180) + CHANNEL1_MIN);
		}
	}
	return move;
}

// Simple Method to set a specific servo to a specific angle; without any Smoothing
void setServoDegree(int channel, int degree) {
	// Calculate the PWM-Signal
	int move = calculateServoPWMSignal(channel, degree);
	
	// Read the previous set from Controller to calculate later the duration for waiting, until the servo is in the right position
	int oldSet = readServoPosition(channel);
	
	// Move the Servo
	pwmWrite(PIN_BASE + channel, move);
	
	// Calculate the waiting duration
	int diffSet;
	if (channel == 0) {
		diffSet = (move - oldSet) * 2.6;
	} else {
		diffSet = (move - oldSet) * 2.4;
	}
	if (diffSet > 0) {
		delay(diffSet);
	} else {
		delay(-diffSet);
	}
}

// Method to set a specific servo to a specifig angle with smoothing
void setServoDegreeSmooth(int channel, int degree) {
	// Read the previous set from Controller to calculate later the duration for waiting, until the servo is in the right position
	int oldSet = readServoPosition(channel);
	
	// Calculate the PWM-Signal
	int move = calculateServoPWMSignal(channel, degree);
	
	// Calculate, how many steps are needed to guarantee a smooth movement
	int diffSet = move - oldSet;
	if (diffSet < 0) {
		diffSet *= -1;
	}
	
	
}

// Moves all Servos to 0 degree (horizontal)
void setServoNull() {
	printf("Setting Servos to Starting Point...\n");
	// Set Pin = 16 to set all Channels / Servos
	setServoDegree(16, 0);
	delay(1000);
}

// First initial setup (connection) for the Gyroscope
void firstSetupGyro() {
	// Connect to Gyroscope on I2C Adress 0x68
	connectionGyroscope = wiringPiI2CSetup (0x68);
	if (connectionGyroscope < 0) {
		printf("Error in Gyroscope Setup\n");
		errorCode = 2;
		printf("\n\nError Code: %d\n", errorCode);
		exit(2);
	} else {
		//disable sleep mode
		wiringPiI2CWriteReg8(connectionGyroscope,0x6B,0x00);
	}
}

// Reads raw values from the Gyroscope
int read_word_2c(int addr) {
	// Some logik from the Internet
	int val;
	val = wiringPiI2CReadReg8(connectionGyroscope, addr);
	val = val << 8;
	val += wiringPiI2CReadReg8(connectionGyroscope, addr+1);
	if (val >= 0x8000) {
		val = -(65536 - val);
	}
	return val;
}

// Simple Method to calculate the distance between to points in a tringle (Pythagoras)
double dist(double a, double b) {
	return sqrt((a*a) + (b*b));
}

// Calculates the y - Rotation of the Gyroscope from the raw Data
double get_y_rotation(double x, double y, double z) {
	// Takes raw data from the Gyroscope and turns it into degree
	double radians;
	radians = atan2(x, dist(y, z));
	return -(radians * (180.0 / M_PI));
}

// Calculates the y - Rotation of the Gyroscope from the raw Data
double get_x_rotation(double x, double y, double z) {
	// Takes raw data from the Gyroscope (Given in the paramter) and turns it into degree
	double radians;
	radians = atan2(y, dist(x, z));
	return -(radians * (180.0 / M_PI));
}

// Corrects the measurement
void correctAngle() {
	// Multiply with a factor to get nearly the right values
	double factor = 1.5;
	gyroscopeXReal *= factor;
	gyroscopeYReal *=  factor;
	
	// Add (or Subtrate) the calculated correction Value for each direction
	gyroscopeXReal += correctionGyroscopeX;
	gyroscopeYReal += correctionGyroscopeY;
}

// Reads from the Gyroscope and writes the "real" angles to the given variables
void getGyroDegree() {
	double accelerationX = read_word_2c(0x3B) / 16384.0;
	double accelerationY = read_word_2c(0x3D) / 16384.0;
	double accelerationZ = read_word_2c(0x3F) / 16384.0;
	
	gyroscopeXReal = get_x_rotation(accelerationX, accelerationY, accelerationZ) - gyroscopeXOffset;
	gyroscopeYReal = get_y_rotation(accelerationX, accelerationY, accelerationZ) - gyroscopeYOffset;
}

// Only for Startup, eliminates Deviation
void setGyroNull() {
	getGyroDegree();
	gyroscopeXOffset = gyroscopeXReal;
	gyroscopeYOffset = gyroscopeYReal;
}

// Startup Function; Calibrates Gyroscope and provides advanced calibration data for correction
void calibrateGyro() {
	// Look Comment in Function
	setGyroNull();
	
	// Variable to set Quality of Correction
	double correctionQuality = 0.5;
	// Variable to set Quantity of Measurement (how many)
	// Only integer divisor of 40
	int correctionSteps = 8;
	// Member-Variable to keep, how many corrections were needed individually
	int correctionGyroscopeAllY[40 / correctionSteps];
	
	
	// Correction for yAxis
	for (int i = correctionSteps; i <= 40; i += correctionSteps) {
		setServoDegree(0, i);
		getGyroDegree();
		correctAngle();
		
		correctionGyroscopeAllY[i / correctionSteps - 1] = 0;

		printf("Uncorrected yAxis: %f     ", gyroscopeYReal);
		// Tests, wether the measurement was too low or to high
		while ((gyroscopeYReal - correctionQuality * 10) > i) {
			gyroscopeYReal -= correctionQuality;
			correctionGyroscopeAllY[i / correctionSteps - 1]--;
		}
		while ((gyroscopeYReal + correctionQuality * 10) < i) {
			gyroscopeYReal += correctionQuality;
			correctionGyroscopeAllY[i / correctionSteps - 1]++;
		}
		printf("Corrected yAxis: %f\n", gyroscopeYReal);
	}
	setServoDegree(0, 0);
	
	// Simple logic to get the average of all Measurements
	int allCorrections = 0;
	for (int i = 0; i < 40 / correctionSteps; i++) {
		allCorrections += correctionGyroscopeAllY[i];
	}
	correctionGyroscopeY = allCorrections / (40 / correctionSteps);
	printf("Correction Steps yAxis: %d\n", correctionGyroscopeY);
	
	
	
	// Same as above, but for yAxis
	int correctionGyroscopeAllX[40 / correctionSteps];
	for (int i = correctionSteps; i <= 40; i += correctionSteps) {
		setServoDegree(1, i);
		getGyroDegree();
		correctAngle();
		
		correctionGyroscopeAllX[i / correctionSteps - 1] = 0;

		printf("Uncorrected xAxis: %f     ", gyroscopeXReal);
		// Tests, wether the measurement was to low or to high
		while ((gyroscopeXReal - correctionQuality * 10) > i) {
			gyroscopeXReal -= correctionQuality;
			correctionGyroscopeAllX[i / correctionSteps - 1]--;
		}
		while ((gyroscopeXReal + correctionQuality * 10) < i) {
			gyroscopeXReal += correctionQuality;
			correctionGyroscopeAllX[i / correctionSteps - 1]++;
		}
		printf("Corrected xAxis: %f\n", gyroscopeXReal);
	}
	setServoDegree(1, 0);
	
	// Simple logic to get the average of all Measurements
	allCorrections = 0;
	for (int i = 0; i < 40 / correctionSteps; i++) {
		allCorrections += correctionGyroscopeAllX[i];
	}
	correctionGyroscopeX = allCorrections / (40 / correctionSteps);
	printf("Correction Steps xAxis: %d\n", correctionGyroscopeX);
	
}

void getServoPosition(int *servoPositionX, int *servoPositionY) {
	// Gets the raw PWM Data from the Servo and decrypts the original angle
	*servoPositionY = readServoPosition(0);
	*servoPositionX = readServoPosition(1);
	*servoPositionX -= CHANNEL1_MIN * 2;
	*servoPositionX *= 180;
	*servoPositionX /= CHANNEL1_DIFF;
	*servoPositionX -= 90;
	*servoPositionY -= CHANNEL0_MIN * 2;
	*servoPositionY *= 180;
	*servoPositionY /= CHANNEL0_DIFF;
	*servoPositionY -= 89;
	
	
	// Correct the values, measured by the Gyroscope
	getGyroDegree();
	correctAngle();
	
	// Control, if the two values match nearly, else write a error code to the DB
	int deviation = 20;
	if ((gyroscopeXReal > (*servoPositionX + deviation)) || (gyroscopeXReal < (*servoPositionX - deviation)) || (gyroscopeYReal > (*servoPositionY + deviation)) || (gyroscopeYReal < (*servoPositionY - deviation))) {
		errorCode = 3;
	}
}

int main(int argc, char **argv) {
	// Connect and calibrate Servos
	firstSetupServo();
	setServoNull();
	
	// Connect and calibrate Gyroscope
	firstSetupGyro();
	calibrateGyro();
	printf("====================\nCalibration finished\n====================\n\n");
	
	int servoPositionX = 0;
	int servoPositionY = 0;
	
	setServoDegreeSmooth(0, 0);
	setServoDegree(0, 20);
	getServoPosition(&servoPositionX, &servoPositionY);
	printf("xDrehung1: %d\nyDrehung1: %d\n", servoPositionX, servoPositionY);
	setServoDegree(0, -20);
	getServoPosition(&servoPositionX, &servoPositionY);
	printf("xDrehung1: %d\nyDrehung1: %d\n", servoPositionX, servoPositionY);
	setServoDegree(0, 0);
	getServoPosition(&servoPositionX, &servoPositionY);
	printf("xDrehung1: %d\nyDrehung1: %d\n", servoPositionX, servoPositionY);
	
	printf("\n\nError Code: %d\n", errorCode);
	return 0;
}

