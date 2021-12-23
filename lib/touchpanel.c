#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#define EVENT_DEVICE    "/dev/input/event0"
#define EVENT_TYPE      EV_ABS
#define EVENT_CODE_X    ABS_X
#define EVENT_CODE_Y    ABS_Y

int connectionTouchpanel = 0;

void firstSetupTouchpanel() {
	printf("Connecting to Touchpanel...\n");
	
}

void checkUser(){
    if ((getuid()) != 0) {
        fprintf(stderr, "---ERROR 3--- You must be on root!\n");
        exit(-3);
    }

}

void getCoordinates(int *posX, int *posY){

    struct input_event ev;
    char name[256] = "";

    int posXold = *posX;
    int posYold = *posY;

    /* Open Device */
    connectionTouchpanel = open(EVENT_DEVICE, O_RDONLY);
    if (connectionTouchpanel == -1) {
        fprintf(stderr, "---ERROR 4--- Can't open device %s!\n", EVENT_DEVICE);
        
    }

    /* Print Device Name */
    ioctl(connectionTouchpanel, EVIOCGNAME(sizeof(name)), name);
    printf("Reading from:\n");
    printf("device file = %s\n", EVENT_DEVICE);
    printf("device name = %s\n", name);

    while(posXold == *posX || posYold == *posY) {
    
        const size_t ev_size = sizeof(struct input_event);
        ssize_t size;

        size = read(connectionTouchpanel, &ev, ev_size);
        if (size < ev_size) {
            fprintf(stderr, "---ERROR 5--- Can't read device");
            goto err;
        }

        if (ev.type == EVENT_TYPE && (ev.code == EVENT_CODE_X || ev.code == EVENT_CODE_Y)) {
            if(ev.code == EVENT_CODE_X){
               
                *posX = ev.value;

            } 
            if(ev.code == EVENT_CODE_Y){
               
                *posY = ev.value;

            }

        }

    }
    
    err:
    close(connectionTouchpanel);

}

   
int main(int argc, char *argv[])
{
    checkUser();

    int posX = 0;
    int posY = 0;

    getCoordinates(&posX, &posY);
    printf("X = %i\n", posX);
    printf("Y = %i\n", posY);

    
}