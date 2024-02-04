#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wiringPi.h>

// For variable "errorCode"
#include "servo.h"
#include "ADS1115.h"

#define POSDEVIATION 580

void firstSetupTouchpanelADC(u_int8_t i2cAddress) {
    initADC(i2cAddress);
}

// Function to read from the ADC and convert it into 2 digital values
void getTouchpanelPositionADC(int* posX, int* posY) {

    float channel0, channel2;

    // Logic for getting the touchpanel-position (https://de.wikipedia.org/wiki/Touchscreen)
    // 1) Initialize the GPIO library
    wiringPiSetup();

    // 2) Set GPIO 21 to 3,3V; GPIO 22 to GND; GPIO 23 & 26 input;
    pinMode(26, INPUT);
    pinMode(23, INPUT);
    pinMode(21, OUTPUT);
    pinMode(22, OUTPUT);
    digitalWrite(21, HIGH);
    digitalWrite(22, LOW);

    // 3) Measure the Voltage on GPIO 26
    channel2 = getVoltage(2);

    // 4) Calculate the distance to the edges of the touchpanel
    // length 304mm , width 228mm
    // Configuration Sebastian & Simon
    //*posX = (channel2 - 0.2) * 1.87 * 304 / 3.3;
    // Configuration Schule Arduino Uno
    //*posX = (channel2 + 0.2) * 1.87 * 304 / 3.3;
    // Configuration Schule Arduino Nano
    *posX = (channel2 + 0.22) * 1.92 * 304 / 3.3;

    // Check if the value is correct and if there is a need: correct
    // No error is given because of the wide range of different:
    // quality touchpanels / cables / solder jonts
    if (*posX < 0) {
        *posX = 0;
    }

    // 5) Set GPIO 23 to 3,3V; GPIO 26 to GND; GPIO 21 & 22 input;
    pinMode(21, INPUT);
    pinMode(22, INPUT);
    pinMode(26, OUTPUT);
    pinMode(23, OUTPUT);
    digitalWrite(23, HIGH);
    digitalWrite(26, LOW);

    // 6) Measure the Voltage on GPIO 22
    channel0 = getVoltage(0);

    // 7) Calculate the distance to the edges of the touchpanel
    // Configuration Simon & Sebastian
    //*posY = -39 + (channel0 - 0.2) * 1.4 * 228 / 3.3;
    // Configuration Schule Arduino Uno
    *posY = (channel0 + 0.2) * 1.4 * 228 / 3.3;
    
    // Check if the value is correct and if there is a need: correct
    // No error is given because of the wide range of different:
    // quality touchpanels / cables / solder jonts
    if (*posY < 0) {
        *posY = 0;
    }

    // 8) Set the GPIOs back to input (safety and power-efficiency)
    pinMode(26, INPUT);
    pinMode(23, INPUT);
}

// Check the position on every edge in order to be able to deal with different touchpanels
// Also eliminate changes caused by corrosion, new cables, ...
void calibrateTouchpanel() {
    // ToDo
}
