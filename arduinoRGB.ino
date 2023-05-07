// Adafruit NeoPixel library

#include <Adafruit_NeoPixel.h>

// On which pin the Arduino is connected to the rgb strip
#define PIN 6

Adafruit_NeoPixel pixels(30, PIN, NEO_GRB + NEO_KHZ800);

int colourRed;
int result;

void setup() {
  pinMode(0, INPUT);    // sets the digital pin 7 as input
  pinMode(1, INPUT);    // sets the digital pin 7 as input
  pinMode(2, INPUT);    // sets the digital pin 7 as input
  
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

void loop() {
  // Get the values sent by the raspberry
  result = digitalRead(0);
  result += digitalRead(1) << 1;
  result += digitalRead(2) << 2;

  // Control the colour according to the sent value (result)
  if(result == 0) {        // 000 -> 0-12,5
    colourRed = 255;
  } else if(result == 1) { // 001 -> 12,5-25
    colourRed = 219;
  } else if(result == 2) { // 010 -> 25-37,5
    colourRed = 182;
  } else if(result == 3) { // 011 -> 37,5-50
    colourRed = 146;
  } else if(result == 4) { // 100 -> 50-62,5
    colourRed = 109;
  } else if(result == 5) { // 101 -> 62,5-75
    colourRed = 73;
  } else if(result ==6) { // 110 -> 75-87,5
    colourRed = 36;
  } else if(result == 7) { // 111 -> 87,5-100
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
