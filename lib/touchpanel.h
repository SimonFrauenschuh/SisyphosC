/* Copyright (C) 2021 Simon Frauenschuh - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */
#include "touchpanel.c"

#pragma once

extern int connectionTouchpanel;

// To use with an USB controller
// Initial function to connect to the Touchpanel
// Read from "Event 0" (Linux-Kernel)
void firstSetupTouchpanelUSB();

// Connect to the ADC (I2C)
void firstSetupTouchpanelADC(u_int8_t i2cAddress);

// To use with an USB controller
// Function to check if the user is root
void checkUser();

// To use with an USB controller
// Function to read from the Touchpanel (driver) and write onto the given Pointers
void getTouchpanelPositionUSB(int *posX, int *posY);

// Function to read from the ADC and convert it into 2 digital values
void getTouchpanelPositionADC(int* posX, int* posY);
