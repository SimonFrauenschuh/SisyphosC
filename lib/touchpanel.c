#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <time.h>
#include <wiringPi.h>

// For variable "errorCode"
#include "servo.h"
#include "ADS1115.h"

#define EVENT_DEVICE "/dev/input/event0"
#define EVENT_TYPE EV_ABS
#define EVENT_CODE_X ABS_X
#define EVENT_CODE_Y ABS_Y

#define POSDEVIATION 580

// USB
int connectionTouchpanel = 0;

// Initial function to connect to the Touchpanel
// Read from "Event 0" (Linux-Kernel)
void firstSetupTouchpanelUSB() {
    printf("Connecting to Touchpanel...\n");

    char name[256] = "";
    // Open Device
    //connectionTouchpanel = open(EVENT_DEVICE, O_RDONLY);
    connectionTouchpanel = open(EVENT_DEVICE, O_RDWR);
    if (connectionTouchpanel == -1) {
        fprintf(stderr, "---ERROR 3--- Can't open device %s!\n", EVENT_DEVICE);
        errorCode = 3;
        exit(3);
    }

    // Read and print Device Name
    ioctl(connectionTouchpanel, EVIOCGNAME(sizeof(name)), name);
    printf("Reading from:\n");
    printf("device file = %s\n", EVENT_DEVICE);
    printf("device name = %s\n\n", name);
}

void firstSetupTouchpanelADC(u_int8_t i2cAddress) {
    initADC(i2cAddress);
}

// To use with an USB controller
// Function to check if the user is root
void checkUser() {
    if ((getuid()) != 0) {
        fprintf(stderr, "---ERROR 6--- You must be on root!\n");
        errorCode = 6;
        exit(6);
    }
}

// Function to read from the Touchpanel (driver) and write onto the given Pointers
void getTouchpanelPositionUSB(int* posX, int* posY) {
    //truncate(EVENT_DEVICE, 0);

    close(connectionTouchpanel);
    connectionTouchpanel = open(EVENT_DEVICE, O_RDWR);

    struct input_event ev;
    // Store the olg (given) values
    int posXold = *posX;
    int posYold = *posY;

    const size_t ev_size = sizeof(struct input_event);
    ssize_t size;

    // Do this, until both values have changed
    while (posXold == *posX || posYold == *posY) {
        // Read from the Event0 file
        size = read(connectionTouchpanel, &ev, ev_size);
        if (size < ev_size) {
            fprintf(stderr, "---ERROR 7--- Can't read device");
            errorCode = 7;
            close(connectionTouchpanel);
            exit(7);
        }
        // If a read value is X or Y jump into and write onto the given Pointers
        if (ev.type == EVENT_TYPE && (ev.code == EVENT_CODE_X || ev.code == EVENT_CODE_Y)) {
            if (ev.code == EVENT_CODE_X) {
                *posX = ev.value - POSDEVIATION;
            }
            if (ev.code == EVENT_CODE_Y) {
                *posY = ev.value - POSDEVIATION;
            }
        }
    }
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
    // Configuration Simon
    *posX = -8 + (channel2 - 0.2) * 2.3 * 304 / 3.3;
    // Configuration Sebastian
    //*posX = -110 + (channel2 - 0.2) * 3.1 * 304 / 3.3;

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
    // Configuration Simon
    *posY = -39 + (channel0 - 0.2) * 1.4 * 228 / 3.3;
    // Configuration Sebastian
    //*posY = -150 + (channel0 - 0.2) * 2.6 * 228 / 3.3;
    
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
