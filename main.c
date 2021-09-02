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

void firstSetupServo() {
	printf("Connecting to Servos (I2C)...\n");
	// Calling wiringPi setup first (Lookup "Setup.txt")
	wiringPiSetup();

	// Setup with pinbase 300 and i2c location 0x41
	connectionServo = pca9685Setup(PIN_BASE, 0x41, HERTZ);
	if (connectionServo < 0)
	{
		printf("Error in setup\n");
		exit(1);
	}
}

void setServoDegree(int channel, int degree) {
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
	// Read the previous set from Controller to calculate the duration for waiting, until the servo is in the right position
	// Move the Servo
	int oldSet = analogRead(PIN_BASE + channel) & 0xFFF;
	pwmWrite(PIN_BASE + channel, move);
	int diffSet = (move - oldSet) * 1.0;
	if (diffSet > 0) {
		delay(diffSet);
	} else {
		delay(-diffSet);
	}
}

void setServoNull() {
	printf("Setting Servos to Starting Point...\n");
	// Reset all output, only for startup
	pca9685PWMReset(connectionGyroscope);
	// Set Pin = 16 to set all Channels / Servos
	setServoDegree(16, 0);
	delay(1000);
}

void firstSetupGyro() {
	// Connect to Gyroscope on I2C Adress 0x68
	connectionGyroscope = wiringPiI2CSetup (0x68);
	//disable sleep mode
	wiringPiI2CWriteReg8(connectionGyroscope,0x6B,0x00);
}

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

double dist(double a, double b) {
	return sqrt((a*a) + (b*b));
}
 
double get_y_rotation(double x, double y, double z) {
	// Takes raw data from the Gyroscope and turns it into degree
	double radians;
	radians = atan2(x, dist(y, z));
	return -(radians * (180.0 / M_PI));
}
 
double get_x_rotation(double x, double y, double z) {
	// Takes raw data from the Gyroscope (Given in the paramter) and turns it into degree
	double radians;
	radians = atan2(y, dist(x, z));
	return (radians * (180.0 / M_PI));
}

void correctAngle(double angle) {
	// Multiply with a factor to get nearly the right values
	double factor = 1.5;
	gyroscopeXReal *= factor;
	gyroscopeYReal *=  factor;
	
	// Add (or Subtrate) the calculated correction Value for each Servo
	gyroscopeXReal += correctionGyroscopeX;
	gyroscopeYReal += correctionGyroscopeY;
	
	// Take the average of the measurement of the Gyroscope and the value of the Servo
	gyroscopeXReal = (gyroscopeXReal + angle) / 2;
	gyroscopeYReal = (gyroscopeYReal + angle) / 2;
}

void correctAngleCalibrationUncallable() {
	double factor = 1.5;
	gyroscopeXReal *= factor;
	gyroscopeYReal *=  factor;
}

void getGyroDegree() {
	// Reads from the Gyroscope and writes the "real" angles to the given variables (Pointer)
	double accelerationX = read_word_2c(0x3B) / 16384.0;
	double accelerationY = read_word_2c(0x3D) / 16384.0;
	double accelerationZ = read_word_2c(0x3F) / 16384.0;
	
	gyroscopeXReal = get_x_rotation(accelerationX, accelerationY, accelerationZ) - gyroscopeXOffset;
	gyroscopeYReal = get_y_rotation(accelerationX, accelerationY, accelerationZ) - gyroscopeYOffset;
}

void setGyroNull() {
	// Only for Startup, eliminates Deviation
	getGyroDegree();
	gyroscopeXOffset = gyroscopeXReal;
	gyroscopeYOffset = gyroscopeYReal;
	getGyroDegree();
	//printf("xAxis: %f\nyAxis: %f\n", gyroscopeXReal, gyroscopeYReal);
}

void calibrateGyro() {
	// Look Comment in Function
	setGyroNull();
	
	// Variable to set Quality of Correction
	double correctionQuality = 0.05;
	// Variable to set Quantity of Measurement (how many)
	// Only integer divisor of 40
	int correctionSteps = 4;
	// Member-Variable to keep, how many corrections were needed individually
	int correctionGyroscopeAllY[40 / correctionSteps - 1][0];
	
	
	// Correction for yAxis
	for (int i = correctionSteps; i <= 40; i += correctionSteps) {
		setServoDegree(0, i);
		getGyroDegree();
		correctAngleCalibrationUncallable();

		printf("Uncorrected yAxis: %f     ", gyroscopeYReal);
		// Tests, wether the measurement was to low or to high
		while ((gyroscopeYReal - correctionQuality * 10) > i) {
			gyroscopeYReal -= correctionQuality;
			correctionGyroscopeAllY[i / correctionSteps - 1][0]--;
		}
		while ((gyroscopeYReal + correctionQuality * 10) < i) {
			gyroscopeYReal += correctionQuality;
			correctionGyroscopeAllY[i / correctionSteps - 1][0]++;
		}
		printf("Corrected yAxis: %f\n", gyroscopeYReal);
	}
	setServoDegree(0, 0);
	
	// Simple logic to get the average of all Measurements
	int allCorrections = 0;
	for (int i = 0; i < 40 / correctionSteps; i++) {
		allCorrections += correctionGyroscopeAllY[i][0];
	}
	correctionGyroscopeY = allCorrections / (40 / correctionSteps);
	printf("Correction Steps yAxis: %d\n", correctionGyroscopeY);
	
	
	
	// Correction for yAxis
	int correctionGyroscopeAllX[40 / correctionSteps - 1][0];
	for (int i = correctionSteps; i <= 40; i += correctionSteps) {
		setServoDegree(1, i);
		getGyroDegree();
		correctAngleCalibrationUncallable();

		printf("Uncorrected xAxis: %f     ", gyroscopeXReal);
		// Tests, wether the measurement was to low or to high
		while ((gyroscopeXReal - correctionQuality * 10) > i) {
			gyroscopeXReal -= correctionQuality;
			correctionGyroscopeAllX[i / correctionSteps - 1][0]--;
		}
		while ((gyroscopeXReal + correctionQuality * 10) < i) {
			gyroscopeXReal += correctionQuality;
			correctionGyroscopeAllX[i / correctionSteps - 1][0]++;
		}
		printf("Corrected xAxis: %f\n", gyroscopeXReal);
	}
	setServoDegree(1, 0);
	
	// Simple logic to get the average of all Measurements
	allCorrections = 0;
	for (int i = 0; i < 40 / correctionSteps; i++) {
		allCorrections += correctionGyroscopeAllX[i][0];
	}
	correctionGyroscopeX = allCorrections / (40 / correctionSteps);
	printf("Correction Steps xAxis: %d\n", correctionGyroscopeX);
	
}

int main(int argc, char **argv) {
	// Connect and calibrate Servos
	firstSetupServo();
	setServoNull();
	
	// Connect and calibrate Gyroscope
	firstSetupGyro();
	calibrateGyro();
	

	setServoDegree(0, 40);
	getGyroDegree();
	correctAngle(40.0);
	printf("xDrehung1: %f\nyDrehung1: %f\n", gyroscopeXReal, gyroscopeYReal);
	setServoDegree(0, 0);
	getGyroDegree();
	correctAngle(0.0);
	printf("xDrehung1: %f\nyDrehung1: %f\n", gyroscopeXReal, gyroscopeYReal);
	
	return 0;
}

