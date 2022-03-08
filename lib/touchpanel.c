#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <time.h>
#include <wiringPi.h>

// For variable "errorCode"
#include "servo.h"
#include "Adafruit_ADS1015.h"

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

void firstSetupTouchpanelADC(uint8_t i2cAddress) {
    // Connect to the right I2C Adress
    begin(i2cAddress);
}

// Function to check if the user is root
void checkUser() {
    if ((getuid()) != 0) {
        fprintf(stderr, "---ERROR 5--- You must be on root!\n");
        errorCode = 5;
        exit(5);
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
            fprintf(stderr, "---ERROR 6--- Can't read device");
            errorCode = 6;
            close(connectionTouchpanel);
            exit(6);
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

    uint16_t channel0, channel1;

    struct timeval begin, end;
	long microseconds;
	// Start measuring time
    gettimeofday(&begin, 0);

    // Logic for getting the touchpanel-position (https://de.wikipedia.org/wiki/Touchscreen)
    // 1) Initialize the GPIO library and set all Pins as input (safety)
    wiringPiSetupGpio();
    pinMode(5, INPUT);
    pinMode(6, INPUT);
    pinMode(12, INPUT);
    pinMode(13, INPUT);

    // 2) Set GPIO 5 & 6 to 3,3V; GPIO 12 & 13 to GND (input);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);

    // 3) Measure the two Voltages
    channel0 = readADC_SingleEnded(0);

    // 4) Calculate the distance to the edges of the touchpanel
    // length 304mm , width 228mm
    posX = channel0 * 1.31578947 * 304 / 3.3;

    // 5) Set GPIO 5 & 6 to GND (input); GPIO 12 & 13 to 3,3V;
    pinMode(5, INPUT);
    pinMode(6, INPUT);
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
    digitalWrite(12, HIGH);
    digitalWrite(13, HIGH);

    // 6) Measure the two Voltages
    channel1 = readADC_SingleEnded(1);

    // 7) Calculate the distance to the edges of the touchpanel
    posY = channel1 * 1.31578947 * 228 / 3.3;

    // 8) Set the GPIOs back to input (safety and power-efficiency)
    pinMode(12, INPUT);
    pinMode(13, INPUT);

    gettimeofday(&end, 0);
    microseconds = end.tv_usec - begin.tv_usec;
    printf("%ld\n", microseconds / 1000);
}