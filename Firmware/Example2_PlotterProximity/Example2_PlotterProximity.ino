/*
  Robotic Finger Sensor - Basic example
  By: Nathan Seidle
  SparkFun Electronics
  Date: June 12th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware License).

  This example outputs the proximity values so the plotter can show them.
  Open the 'Serial Plotter' at 9600bps and you should see the graph of proximity.

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

void setup()
{
  Serial.begin(9600);
  Serial.println("Robotic Finger Sensor v2 Example");

  Wire.begin();
  Wire.setClock(400000); //Increase I2C bus speed to 400kHz

  proximitySensor.begin();
  pressureSensor.begin();
}

void loop()
{
  unsigned int proxValue = proximitySensor.getProximity(); 

  Serial.print(proxValue);
  Serial.print(",");

  Serial.println();

  delay(10);
}

