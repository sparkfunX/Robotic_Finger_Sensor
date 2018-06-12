/*
  Robotic Finger Sensor - Basic examples
  By: Nathan Seidle
  SparkFun Electronics
  Date: June 12th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware License).

  Outputs the IR Proximity Value from the VCNL4040 sensor and the pressure reading and
  temp from the LPS25HB pressure sensor.

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

  if (proximitySensor.begin() == false)
  {
    Serial.println("Proximity sensor not found. Please check wiring.");
    while (1); //Freeze!
  }
  
  pressureSensor.begin();
  if(pressureSensor.isConnected() == false)  // The library supports some different error codes such as "DISCONNECTED"
  {
    Serial.println("Pressure sensor not found. Please check wiring.");
    while (1); //Freeze!
  }
}

void loop()
{
  unsigned int proxValue = proximitySensor.getProximity(); 

  float pressure = pressureSensor.getPressure_hPa();
  float temp = pressureSensor.getTemperature_degC();

  Serial.print("Proximity Value: ");
  Serial.print(proxValue);

  Serial.print(" Pressure in hPa: "); 
  Serial.print(pressure);
  Serial.print(", Temperature (degC): "); 
  Serial.println(temp);

  delay(10);
}

