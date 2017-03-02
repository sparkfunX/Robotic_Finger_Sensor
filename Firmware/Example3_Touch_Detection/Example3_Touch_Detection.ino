/*
  VCNL4040 Proximity and Ambient Light Sensor - Basic test code example
  By: Rob Reynolds
  SparkFun Electronics
  Date: February 14th, 2017
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware License).

  This code is heavily based on Robotic Material's force proximity eval code: https://github.com/RoboticMaterials/FA-I-sensor
  It has been modified to work with the VCNL4040 sensor.

  This code demonstrates the force response of the RM Pressure Sensor eval kit.

  The output will be distance reading and a touch or release character.

  Press c to turn off continuous mode.
  Press h for help.

  This code works on an Arduino Uno and Teensy 

  Brought to you by SparkFun (orignial code), the Correll Lab at the University
  of Colorado, Boulder and Robotic Materials Inc.

  This software is open source and can be used for any purpose.
*/

/***** Library parameters ****/
#define WIRE Wire //For teensyLC and Arduino Uno
//#define WIRE Wire1 //For others

#include <Wire.h> //For Arduino Uno
//#include <i2c_t3.h> //Use <i2c_t3.h> for Teensy

#include <math.h>

#define LOOP_TIME 10  // loop duration in ms

// Touch/release detection
#define EA 0.3  // exponential average weight parameter / cut-off frequency for high-pass filter

/***** GLOBAL VARIABLES *****/
unsigned int proximity_value; // current proximity reading
unsigned int average_value;   // low-pass filtered proximity reading
signed int fa2;              // FA-II value;
signed int fa2derivative;     // Derivative of the FA-II value;
signed int fa2deriv_last;     // Last value of the derivative (for zero-crossing detection)
signed int sensitivity = 50;  // Sensitivity of touch/release detection, values closer to zero increase sensitivity

int continuous_mode = 1; //Default on
int single_shot = 0;
int touch_analysis = 1; //Default on

void setup()
{
  Serial.begin(115200);
  Serial.println("Robotic finger sensor eval online");

  WIRE.begin();

  delay(1000);

  if (testVCNL4040() == false)
  {
    Serial.println("Device not found. Check wiring.");
    while (1); //Freeze!
  }
  Serial.println("VCNL4040 detected!");

  initVCNL4040(); //Configure sensor

  delay(100);
  proximity_value = readProximity(); //Get proximity values
  average_value = proximity_value;
  fa2 = 0;
}

void loop()
{
  unsigned long startTime = millis();

  if (Serial.available()) {
    Serial.println("Received!");
    char cmd = Serial.read(); // read the incoming byte
    switch (cmd) {
      case 's' :
        single_shot = 1;
        break;
      case 't' :
        if (touch_analysis == 0) touch_analysis = 1;
        else touch_analysis = 0;
        break;
      case 'c' :
        if (continuous_mode == 0) continuous_mode = 1;
        else continuous_mode = 0;
        break;
      case 'h' :
        Serial.println("c: Toggle continuous mode");
        Serial.println("s: Single-shot measurement");
        Serial.println("t: Toggle touch/release analysis");
        break;
    }

  }

  proximity_value = readProximity(); //Get proximity values
  fa2deriv_last = fa2derivative;
  fa2derivative = (signed int) average_value - proximity_value - fa2;
  fa2 = (signed int) average_value - proximity_value;

  if (continuous_mode || single_shot) {
    Serial.print(proximity_value);
    Serial.print(",");
    Serial.print(fa2);
    Serial.print(",");
    //Serial.print(",");
    //Serial.print(fa2derivative);

    if (touch_analysis) {
      if ((fa2deriv_last < -sensitivity && fa2derivative > sensitivity) || (fa2deriv_last > 50 && fa2derivative < -50)) { // zero crossing detected
        // Serial.print(proximity_value); Serial.print(","); Serial.print(fa2); Serial.print(","); Serial.println(fa2derivative);
        if (fa2 < -sensitivity) // minimum
        {
          Serial.print("T");
        }
        else if (fa2 > sensitivity) // maximum
        {
          Serial.print("R");
        }
      }
      else
        Serial.print("0");
    }
  }

  if (continuous_mode || single_shot) {
    single_shot = 0;
    Serial.println();
  }

  // Do this last
  average_value = EA * proximity_value + (1 - EA) * average_value;
  while (millis() < startTime + LOOP_TIME); // enforce constant loop time
}

