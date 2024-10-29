/***************************************************************************
  This is a proof of concept RGB light meter intended for darkroom enlarger
  readings.

  Designed to work with:
  
  An Adafruit AS7262 breakout:
  ----> http://www.adafruit.com/products/3779

  A Seed Studio XIAO 
  ----> https://www.seeedstudio.com/xiao-series-page

  And a small OLED display like
  ----> https://www.amazon.com/Hosyond-Display-Self-Luminous-Compatible-Raspberry/dp/B09T6SJBV5/ref=sr_1_2


  These sensors use I2C to communicate. The addresses used by your device may be
  different from what is in this sketch.

  The wiring is very simple with all devide communication taking place over I2C.

 ***************************************************************************/

#include <Wire.h>
#include "Adafruit_AS726x.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_FeatherOLED.h>

Adafruit_AS726x ams;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_FeatherOLED oled = Adafruit_FeatherOLED();

float averageCalibratedRGB[3];
float densityRGB[3];
float priorDensityRGB[3];

const int NUM_AVERAGE_SAMPLES = 1;

void setup() {
  // Serial.begin(9600);
  // while(!Serial);
  
  oled.init();
  // oled.setBatteryVisible(true);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  //begin and make sure we can talk to the sensor
  if(!ams.begin()){
    Serial.println("could not connect to sensor! Please check your wiring.");
    while(1);
  }
}

float round2(float value) {
  return roundf(value * 100) / 100;
}

void read_calibrated_rgb(int samples) {
  float calibratedValues[AS726x_NUM_CHANNELS];

  float currentCalibratedRGB[3] = {0.0, 0.0, 0.0};

  for(int i = 0; i < samples; i++) {
      ams.setIntegrationTime(255);
      ams.startMeasurement(); //begin a measurement
      
      //wait till data is available
      bool rdy = false;
      while(!rdy){
        delay(1);
        rdy = ams.dataReady();
      }
      
      ams.readCalibratedValues(calibratedValues);

      currentCalibratedRGB[0] += calibratedValues[AS726x_RED];
      currentCalibratedRGB[1] += calibratedValues[AS726x_GREEN];
      currentCalibratedRGB[2] += calibratedValues[AS726x_VIOLET];

  }

  averageCalibratedRGB[0] = currentCalibratedRGB[0] / samples;
  averageCalibratedRGB[1] = currentCalibratedRGB[1] / samples;
  averageCalibratedRGB[2] = currentCalibratedRGB[2] / samples;

  priorDensityRGB[0] = densityRGB[0];
  priorDensityRGB[1] = densityRGB[1];
  priorDensityRGB[2] = densityRGB[2];

  densityRGB[0] = averageCalibratedRGB[0] > 1 ? round2(log2f(averageCalibratedRGB[0])) : 0.0;
  densityRGB[1] = averageCalibratedRGB[1] > 1 ? round2(log2f(averageCalibratedRGB[1])) : 0.0;
  densityRGB[2] = averageCalibratedRGB[2] > 1 ? round2(log2f(averageCalibratedRGB[2])) : 0.0;
}

void update_density_display() {
  if ((densityRGB[0] != priorDensityRGB[0]) || (densityRGB[1] != priorDensityRGB[1]) || (densityRGB[2] != priorDensityRGB[2])) {
    // clear the current values
    oled.clearDisplay();
    oled.clearMsgArea();

    oled.printf("R: %2.2f   %5.0f\n", densityRGB[0], averageCalibratedRGB[0]);
    oled.printf("G: %2.2f   %5.0f\n", densityRGB[1], averageCalibratedRGB[1]);
    oled.printf("B: %2.2f   %5.0f\n", densityRGB[2], averageCalibratedRGB[2]);

    oled.display();
  }
}

void loop() {
  read_calibrated_rgb(NUM_AVERAGE_SAMPLES);
  update_density_display();
}
