/*
   Basic test of the VCNL4040 sensor
   By: Nathan Seidle
   SparkFun Electronics
   Date: March 10th, 2017
   License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware License).

   Used to do prelim test of production units.

   Serial.print at 9600 baud to serial monitor.
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
}

void loop()
{

  if (proximitySensor.begin() == true)
    Serial.print("Proximity good ");
  else
    Serial.print("Proximity not found.");

  pressureSensor.begin();
  if (pressureSensor.isConnected() == true)
    Serial.print(" Pressure good");
  else
    Serial.print(" Pressure not found.");

  while (1)
  {
    if (proximitySensor.begin() == false) break; //No more sensor!
    if (pressureSensor.isConnected() == false) break;

    unsigned int proxValue = proximitySensor.getProximity();

    float pressure = pressureSensor.getPressure_hPa();
    float temp = pressureSensor.getTemperature_degC();

    Serial.print("Prox: ");
    Serial.print(proxValue);

    Serial.print(" Pressure: ");
    Serial.print(pressure);
    Serial.print(", Temp(C): ");
    Serial.print(temp);
    Serial.println();

    delay(10);
  }

  Serial.println();
  delay(250);
}
