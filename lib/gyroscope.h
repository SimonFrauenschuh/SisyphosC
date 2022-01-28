/* Copyright (C) 2021 Simon Frauenschuh & Philip Jessner - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */
#include "gyroscope.c"

#pragma once

extern int connectionGyroscope;

extern double gyroscopeXOffset, gyroscopeYOffset;
extern double gyroscopeXReal, gyroscopeYReal;
extern int correctionGyroscopeX, correctionGyroscopeY;

// First initial setup (connection) for the Gyroscope
void firstSetupGyro();

// Reads raw values from the Gyroscope
int readWord2c(int addr);

// Simple Method to calculate the distance between to points in a triangle (Pythagoras)
double dist(double a, double b);

// Calculates the y - Rotation of the Gyroscope from the raw Data
double get_y_rotation(double x, double y, double z);

// Calculates the y - Rotation of the Gyroscope from the raw Data
double get_x_rotation(double x, double y, double z);

// Corrects the measurement
void correctAngle();

// Reads from the Gyroscope and writes the "real" angles to the given variables
void getGyroDegree();

// Only for Startup, eliminates Deviation
void setGyroNull();

// Startup Function; Calibrates Gyroscope and provides advanced calibration data for correction
void calibrateGyro();
