/**************************************************************************/
/*!
    @file     Adafruit_ADS1015.h
    @author   K. Townsend (Adafruit Industries)
    @license  BSD (see license.txt)
    This is a library for the Adafruit ADS1015 breakout board
    ----> https://www.adafruit.com/products/???
    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!
    @section  HISTORY
    v1.0  - First release
    v1.1  - Added ADS1115 support - W. Earl
*/
/**************************************************************************/
#pragma once

#include <stdint.h>
#include <stdio.h>

#include "Adafruit_ADS1015.cpp"

void begin(void);
uint16_t  readADC_SingleEnded(uint8_t channel);
void      updateWiringPiSetup();
void      setI2cAddress(uint8_t i2cAddress);
void      setGain(adsGain_t gain);
adsGain_t getGain(void);