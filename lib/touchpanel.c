#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#include "servo.h"

#define EVENT_DEVICE "/dev/input/event0"
#define EVENT_TYPE EV_ABS
#define EVENT_CODE_X ABS_X
#define EVENT_CODE_Y ABS_Y

#define POSDEVIATION 580

int connectionTouchpanel = 0;

// Initial function to connect to the Touchpanel
// Read from "Event 0" (Linux-Kernel)
void firstSetupTouchpanel()
{
    printf("Connecting to Touchpanel...\n");

    char name[256] = "";
    // Open Device
    //connectionTouchpanel = open(EVENT_DEVICE, O_RDONLY);
    connectionTouchpanel = open(EVENT_DEVICE, O_RDWR);
    if (connectionTouchpanel == -1)
    {
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

// Function to check if the user is root
void checkUser()
{
    if ((getuid()) != 0)
    {
        fprintf(stderr, "---ERROR 5--- You must be on root!\n");
        errorCode = 5;
        exit(5);
    }
}

// Function to read from the Touchpanel (driver) and write onto the given Pointers
void getTouchpanelPosition(int *posX, int *posY)
{
    close(connectionTouchpanel);
    connectionTouchpanel = open(EVENT_DEVICE, O_RDWR);

    struct input_event ev;
    // Store the olg (given) values
    int posXold = *posX;
    int posYold = *posY;

    const size_t ev_size = sizeof(struct input_event);
    ssize_t size;
    // Do this, until both values have changed
    while (posXold == *posX || posYold == *posY)
    {

        // Read from the Event0 file
        size = read(connectionTouchpanel, &ev, ev_size);
        if (size < ev_size)
        {
            fprintf(stderr, "---ERROR 6--- Can't read device");
            errorCode = 6;
            close(connectionTouchpanel);
            exit(6);
        }
        // If a read value is X or Y jump into and write onto the given Pointers
        if (ev.type == EVENT_TYPE && (ev.code == EVENT_CODE_X || ev.code == EVENT_CODE_Y))
        {
            if (ev.code == EVENT_CODE_X)
            {
                *posX = ev.value - POSDEVIATION;
            }
            if (ev.code == EVENT_CODE_Y)
            {
                *posY = ev.value - POSDEVIATION;
            }
        }
    }
}
