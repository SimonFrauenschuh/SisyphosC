// Adafruit NeoPixel library

#include <Adafruit_NeoPixel.h>

// On which pin the Arduino is connected to the rgb strip
#define PIN 6

Adafruit_NeoPixel pixels(30, PIN, NEO_GRB + NEO_KHZ800);

int colourRed;
int lsb, var, msb;

void setup() {
  pinMode(0, INPUT);    // sets the digital pin 7 as input
  pinMode(1, INPUT);    // sets the digital pin 7 as input
  pinMode(2, INPUT);    // sets the digital pin 7 as input
  
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

void loop() {
  // Get the values sent by the raspberry
  lsb = digitalRead(0);
  var = digitalRead(1);
  msb = digitalRead(2);

  // Control the colour according to the sent value (result)
  if(msb==0 && var==0 && lsb == 0) {        // 000 -> 0-12,5
    colourRed = 255;
  } else if(msb==0 && var==0 && lsb == 1) { // 001 -> 12,5-25
    colourRed = 219;
  } else if(msb==0 && var==1 && lsb == 0) { // 010 -> 25-37,5
    colourRed = 182;
  } else if(msb==0 && var==1 && lsb == 1) { // 011 -> 37,5-50
    colourRed = 146;
  } else if(msb==1 && var==0 && lsb == 0) { // 100 -> 50-62,5
    colourRed = 109;
  } else if(msb==1 && var==0 && lsb == 1) { // 101 -> 62,5-75
    colourRed = 73;
  } else if(msb==1 && var==1 && lsb == 0) { // 110 -> 75-87,5
    colourRed = 36;
  } else if(msb==1 && var==1 && lsb == 1) { // 111 -> 87,5-100
    colourRed = 0;
  }
  
  for(int i=0; i<30; i++) { // For each pixel
    // Set the brightness low to save energy (and be able to use a smaller power adapter
    pixels.setBrightness(50);
    // Set the colour for each pixel according to the previous calculated value
    pixels.setPixelColor(i, pixels.Color(colourRed, 255-colourRed, 0));
  }
  pixels.show();   // Send the updated pixel colors to the hardware.
}
