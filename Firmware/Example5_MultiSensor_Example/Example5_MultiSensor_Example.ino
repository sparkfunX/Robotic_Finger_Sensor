/*
  Controlling multiple Robotic Finger Sensors VCNL4040 with PCA9548A/TCA9548A
  By: Nathan Seidle and Nikolaus Correll
  SparkFun Electronics
  Date: April 4th, 2017
  License: This software is open source and can be used for any purpose.

  This code demonstrates how to interact with multiple finger sensors
  using the PCA9548A I2C multiplexer.

  Open the 'Serial Plotter' at 115200bps and you should see the graph of proximity and touch detection.

  Open the terminal window at 115200bps and you should see the raw proximity readings as well as the characters
  'T' and 'R' when a touch or release is detected.

  Press c to turn off continuous mode.
  Press h for help.

  This code works on an Arduino Uno and Teensy

  This code is heavily based on Robotic Material's force proximity eval code: https://github.com/RoboticMaterials/FA-I-sensor
  It has been modified to work with the VCNL4040 sensor. Brought to you by SparkFun (orignial code), the
  Correll Lab at the University of Colorado, Boulder and Robotic Materials Inc.
*/

#define WIRE Wire //For teensyLC and Arduino Uno
//#define WIRE Wire1 //For others

#include <Wire.h> //For Arduino Uno
//#include <i2c_t3.h> //Use <i2c_t3.h> for Teensy

#include <math.h>

#define LOOP_TIME 10  // loop duration in ms

// Touch/release detection
#define EA 0.3  // exponential average weight parameter / cut-off frequency for high-pass filter

/***** GLOBAL VARIABLES *****/
#define NUMBER_OF_SENSORS 2
unsigned int proximity_value[NUMBER_OF_SENSORS]; // current proximity reading
unsigned int average_value[NUMBER_OF_SENSORS];   // low-pass filtered proximity reading
signed int fa2[NUMBER_OF_SENSORS];              // FA-II value
signed int fa2derivative[NUMBER_OF_SENSORS];     // Derivative of the FA-II value;
signed int fa2deriv_last[NUMBER_OF_SENSORS];     // Last value of the derivative (for zero-crossing detection)
signed int sensitivity[NUMBER_OF_SENSORS];  // Sensitivity of touch/release detection, values closer to zero increase sensitivity

int touch_analysis = 1; //Default on

void setup()
{
  Serial.begin(115200);
  Serial.println("Robotic finger sensor eval online");

  WIRE.begin();

  for (int x = 0 ; x < NUMBER_OF_SENSORS ; x++)
  {
    enableMuxPort(x);

    Serial.print("Finger ");
    Serial.print(x);
    if (testVCNL4040() == false)
      Serial.println(" failed to init. Check wiring.");
    else
      Serial.println(" online!");

    initVCNL4040(); //Configure sensor

    delay(100);
    proximity_value[x] = readProximity(); //Get proximity values
    average_value[x] = proximity_value[x];
    fa2[x] = 0;

    sensitivity[x] = 50; //Set sensitivity for every finger to 50.

    disableMuxPort(x);
  }
}

void loop()
{
  unsigned long startTime = millis();

  for (int x = 0 ; x < NUMBER_OF_SENSORS ; x++)
  {
    enableMuxPort(x); //Talk to this specific finger, and only this finger

    proximity_value[x] = readProximity(); //Get proximity values
    fa2deriv_last[x] = fa2derivative[x];
    fa2derivative[x] = (signed int) average_value[x] - proximity_value[x] - fa2[x];
    fa2[x] = (signed int) average_value[x] - proximity_value[x];

    Serial.print("Finger: ");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(proximity_value[x]);
    Serial.print(", ");
    Serial.print(fa2[x]);
    Serial.print(", ");

    //Serial.print(",");
    //Serial.print(fa2derivative);

    if ((fa2deriv_last[x] < -sensitivity[x] && fa2derivative[x] > sensitivity[x]) || (fa2deriv_last > sensitivity[x] && fa2derivative < -sensitivity[x])) { // zero crossing detected
      // Serial.print(proximity_value); Serial.print(","); Serial.print(fa2); Serial.print(","); Serial.println(fa2derivative);
      if (fa2[x] < -sensitivity[x]) // minimum
      {
        Serial.print("Touch!");
      }
      else if (fa2[x] > sensitivity[x]) // maximum
      {
        Serial.print("Release...");
      }
    }
    else
      Serial.print("0");

    Serial.println();

    average_value[x] = EA * proximity_value[x] + (1 - EA) * average_value[x];

    disableMuxPort(x); //Stop talking to this finger
  }

  // Do this last
  while (millis() < startTime + LOOP_TIME); // enforce constant loop time
}

