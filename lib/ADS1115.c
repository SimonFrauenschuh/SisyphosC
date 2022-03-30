// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// ADS1115
// This code is designed to work with the ADS1115_I2CADC I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Analog-Digital-Converters?sku=ADS1115_I2CADC#tabs-0-product_tabset-2

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "ADS1115.h"
// For errorCode
#include "servo.h"

// Function to create the connection via the I2C-Bus
void initADC() {
    // Create I2C bus
	char *bus = "/dev/i2c-1";
	if ((connectionADC = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
        errorCode = 3;
		exit(3);
	}
	// Get I2C device, ADS1115 I2C address is 0x48(72)
	ioctl(connectionADC, I2C_SLAVE, 0x48);

    // Config for AIN0
    // Select configuration register(0x01)
    config[0][0] = 0x01;
    // 0x82 --> 1100 0010 --> 7:Start 6-4:Select Channel 1:Voltage 4V
	config[0][1] = 0xC2;
    // 0xE3 --> 1110 0011 --> 7-5:Speed 1-0:Disable Comparator
	config[0][2] = 0xE3;

    // Config for AIN1
    // Select configuration register(0x01)
    config[1][0] = 0x01;
    // 0x82 --> 1101 0010 --> 7:Start 6-4:Select Channel 1:Voltage 4V
	config[1][1] = 0xD4;
    // 0xE3 --> 1110 0011 --> 7-5:Speed 1-0:Disable Comparator
	config[1][2] = 0xE3;

    // Config for AIN2
    // Select configuration register(0x01)
    config[2][0] = 0x01;
    // 0x82 --> 1110 0010 --> 7:Start 6-4:Select Channel 1:Voltage 4V
	config[2][1] = 0xE4;
    // 0xE3 --> 1110 0011 --> 7-5:Speed 1-0:Disable Comparator
	config[2][2] = 0xE3;

    // Config for AIN3
    // Select configuration register(0x01)
    config[3][0] = 0x01;
    // 0x82 --> 1111 0010 --> 7:Start 6-4:Select Channel 1:Voltage 4V
	config[3][1] = 0xF4;
    // 0xE3 --> 1110 0011 --> 7-5:Speed 1-0:Disable Comparator
	config[3][2] = 0xE3;
}

// Function to get the digital value from the given channel (0-3)
float getVoltage(int channel) {
    // Get the configuration for the chosen Channel
    char configChannel[3] = {0};
    configChannel[0] = config[channel][0];
    configChannel[1] = config[channel][1];
    configChannel[2] = config[channel][2];
    // Write onto the ADC via I2C
	write(connectionADC, configChannel, 3);
	sleep(1);

	// Read 2 bytes of data from data-register(0x00)
	// 1:msb, 2:lsb
	write(connectionADC, 0x00, 1);
	char data[2]={0};
    read(connectionADC, data, 2);
    // Combine the two bytes into a single 16 bit result
    int16_t val = data[0] << 8 | data[1];

    return (float)(val*4.096/32767.0);
}