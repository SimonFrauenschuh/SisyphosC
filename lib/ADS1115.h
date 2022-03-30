/* Copyright (C) 2021 Simon Frauenschuh & Philip Jessner - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */
#pragma once

#include <stdlib.h>

// Value to store the I2C connection
int connectionADC;
// Array to store all Config-Registers
char config[4][3] = {0};

// Function to create the connection via the I2C-Bus
void initADC();

// Function to get the digital value from the given channel (0-3)
float getVoltage(int channel);
