/*
  Proximity Sensing with the VCNL4040 IR based sensor
  By: Nathan Seidle
  SparkFun Electronics
  Date: April 17th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware License).

  This example outputs ambient light readings to the terminal. I'm not sure how this
  would be helpful on a robotic finger sensor but it is an available sensor...
  
  Point the sensor up and start the sketch. Then cover the sensor with your hand.
  The readings decrease in value because there is less light detected.

  Hardware Connections:
  Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the sensor onto the shield
  Serial.print it out at 9600 baud to serial monitor.
*/

#include <Wire.h>

#include "SparkFun_VCNL4040_Arduino_Library.h" //Library: http://librarymanager/All#SparkFun_VCNL4040
VCNL4040 proximitySensor;


void setup()
{
  Serial.begin(9600);
  Serial.println("Robitic Finger Sensor v2 Example");

  Wire.begin();
  Wire.setClock(400000); //Optional but runs I2C bus at faster 400kHz

  proximitySensor.begin(); //Initialize the sensor

  proximitySensor.powerOnAmbient(); //Power up the ambient sensor
}

void loop()
{
  unsigned int ambientValue = proximitySensor.getAmbient(); 
  unsigned int proxValue = proximitySensor.getProximity(); 

  Serial.print("Proximity Value: ");
  Serial.print(proxValue);

  Serial.print(" Ambient light level: ");
  Serial.print(ambientValue);

  Serial.println();

  delay(100);
}

