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

const int LED = 13;

bool proximityPass = false;
bool pressurePass = false;

void setup()
{
  Serial.begin(115200);
  Serial.println("Robotic Finger Sensor v2 Example");

  pinMode(LED, OUTPUT);

  Wire.begin();
}

void loop()
{

  if (proximitySensor.begin() == true)
  {
    Serial.print("Proximity good ");
    proximityPass = true;
  }
  else
  {
    Serial.print("Proximity not found.");
    proximityPass = false;
  }

  pressureSensor.begin();
  if (pressureSensor.isConnected() == true)
  {
    Serial.print(" Pressure good");
    pressurePass = true;
  }
  else
  {
    Serial.print(" Pressure not found.");
    pressurePass = false;
  }

  while (1)
  {
    if (proximitySensor.begin() == false) break; //No more sensor!
    if (pressureSensor.isConnected() == false) break;

    proximitySensor.setLEDCurrent(50); //Set the current used to drive the IR LED - 50mA to 200mA is allowed.
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

    if (pressure > 810 && pressure < 890 && proxValue < 22000)
      digitalWrite(LED, HIGH);
    else
      digitalWrite(LED, LOW);

    delay(100);
  }

  Serial.println();
  delay(100);
  digitalWrite(LED, LOW);
}
