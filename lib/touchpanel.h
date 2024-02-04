/* Copyright (C) 2021 Simon Frauenschuh & Philip Jessner - All Rights Reserved
 * You may use and / or modify this code in
 * terms of private use.
 * Any caused damage or misbehaviour of any components are
 * under the responsibility of the user and and the editor
 * cannot be prosecuted for it
 */
#include "touchpanel.c"

#pragma once

// Connect to the ADC (I2C)
void firstSetupTouchpanelADC(u_int8_t i2cAddress);

// Function to read from the ADC and convert it into 2 digital values
void getTouchpanelPositionADC(int* posX, int* posY);

// Check the position on every edge in order to be able to deal with different touchpanels
// Also eliminate changes caused by corrosion, new cables, ...
void calibrateTouchpanel();