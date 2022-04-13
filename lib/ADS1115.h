/* Copyright (C) 2021 Simon Frauenschuh & Philip Jessner - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */
#pragma once

#include "ADS1115.c"

// Value to store the I2C connection
extern int connectionADC;
// Array to store all Config-Registers
extern u_int8_t config[4][3];

// Function to create the connection via the I2C-Bus
void initADC(u_int8_t i2cAddress);

// Function to get the digital value from the given channel (0-3)
float getVoltage(int channel);
