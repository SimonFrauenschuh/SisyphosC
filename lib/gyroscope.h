/* Copyright (C) 2021 Simon Frauenschuh - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */

#include <math.h>
#include <wiringPiI2C.h>

int connectionGyroscope;

double gyroscopeXOffset = 0, gyroscopeYOffset = 0;
double gyroscopeXReal, gyroscopeYReal;
int correctionGyroscopeX = 0, correctionGyroscopeY = 0;

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
int readWord2c(int addr) {
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
	double accelerationX = readWord2c(0x3B) / 16384.0;
	double accelerationY = readWord2c(0x3D) / 16384.0;
	double accelerationZ = readWord2c(0x3F) / 16384.0;
	
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
