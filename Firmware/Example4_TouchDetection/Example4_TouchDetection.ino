/*
  Robotic Finger Sensor - Basic example
  By: Nathan Seidle
  SparkFun Electronics
  Date: June 12th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware License).

  This code demonstrates the a method for detecting touches using the proximity sensor.

  Open the 'Serial Plotter' at 9600bps and you should see the graph of proximity and touch detection.

  Hardware Connections:
  If needed, attach a Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the device into an available Qwiic port
  Open the serial monitor at 9600 baud to see the output
*/

#include <Wire.h>

#include "SparkFun_VCNL4040_Arduino_Library.h" //Library: http://librarymanager/All#SparkFun_VCNL4040
VCNL4040 proximitySensor;

#include "SparkFun_LPS25HB_Arduino_Library.h"  //Library: http://librarymanager/All#SparkFun_LPS25HB
LPS25HB pressureSensor;

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

void setup()
{
  Serial.begin(9600);
  Serial.println("Robotic Finger Sensor v2 Example");

  Wire.begin();
  Wire.setClock(400000); //Increase I2C bus speed to 400kHz

  if (proximitySensor.begin() == false)
  {
    Serial.println("Proximity sensor not found. Please check wiring.");
    while (1); //Freeze!
  }

  pressureSensor.begin();
  if (pressureSensor.isConnected() == false) // The library supports some different error codes such as "DISCONNECTED"
  {
    Serial.println("Pressure sensor not found. Please check wiring.");
    while (1); //Freeze!
  }

  //Setup initial averages and values
  proximity_value = proximitySensor.getProximity(); //Get proximity values
  average_value = proximity_value;
  fa2 = 0;
}

void loop()
{
  unsigned long startTime = millis();

  proximity_value = proximitySensor.getProximity(); //Get proximity values
  fa2deriv_last = fa2derivative;
  fa2derivative = (signed int) average_value - proximity_value - fa2;
  fa2 = (signed int) average_value - proximity_value;

  Serial.print(proximity_value);
  Serial.print(",");
  Serial.print(fa2);
  Serial.print(",");

  //Serial.print(",");
  //Serial.print(fa2derivative);

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

  Serial.println();

  // Do this last
  average_value = EA * proximity_value + (1 - EA) * average_value;
  while (millis() < startTime + LOOP_TIME); // enforce constant loop time
}

