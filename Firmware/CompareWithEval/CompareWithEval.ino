/*
   VCNL4040 Proximity and Ambient Light Sensor - Basic test code example
   By: Rob Reynolds
   SparkFun Electronics
   Date: February 14th, 2017
   License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware License).

   Simple proximity test.

   Serial.print at 9600 baud to serial monitor.
*/

#include <Wire.h>

#define VCNL4040_ADDR 0x60 //7-bit unshifted I2C address of VCNL4040

//Command Registers have an upper byte and lower byte. 
#define PS_CONF1 0x03
//#define PS_CONF2 //High byte of PS_CONF1
#define PS_CONF3 0x04
//#define PS_MS //High byte of PS_CONF3
#define PS_DATA_L 0x08
//#define PS_DATA_M //High byte of PS_DATA_L
#define ID  0x0C

void setup()
{
  Serial.begin(115200);
  Serial.println("Hello, world!");

  Wire.begin(); //Join i2c bus

  int deviceID = readFromCommandRegister(ID);
  if(deviceID != 0x186)
  {
    Serial.println("Device not found. Check wiring.");
    Serial.print("Expected: 0x186. Heard: 0x");
    Serial.println(deviceID, HEX);
    while(1); //Freeze!
  }
  Serial.println("VCNL4040 detected!");

  initVCNL4040(); //Configure sensor
}

void loop()
{
  int distance = readFromCommandRegister(PS_DATA_L); //Get proximity values

  Serial.print("distance: ");
  Serial.print(distance);
  Serial.println();

  delay(10);
}

//Configure the various parts of the sensor
void initVCNL4040()
{
  //writeByte(AMBIENT_PARAMETER, 0x7F); //Turn on auto offset, average 128 conversions
  //writeByte(IR_CURRENT, ir_current_); //Set to 80mA
  //writeByte(PROXIMITY_MOD, 1); // 1 recommended by Vishay
  
  //Clear PS_SD to turn on proximity sensing
  writeToCommandRegister(PS_CONF1, 0x00, 0x00); //Command register, low byte, high byte
  
  //Set the options for PS_CONF3 and PS_MS bytes
  byte conf3 = 0x00;
  //byte ms = 0b00000010; //Set IR LED current to 100mA
  //byte ms = 0b00000110; //Set IR LED current to 180mA
  byte ms = 0b00000111; //Set IR LED current to 180mA
  writeToCommandRegister(PS_CONF3, conf3, ms);
}

//Reads a two byte value from a command register
unsigned int readFromCommandRegister(byte commandCode)
{
  Wire.beginTransmission(VCNL4040_ADDR);
  Wire.write(commandCode);
  Wire.endTransmission(false); //Send a restart command. Do not release bus.

  Wire.requestFrom(VCNL4040_ADDR, 2); //Command codes have two bytes stored in them

  while(!Wire.available()) delay(1);

  byte lowVal = Wire.read();
  byte highVal = Wire.read();

  return ((unsigned int)(highVal) << 8 | lowVal);
}

//Write a value to a spot
void writeByte(byte addr, byte val)
{
  Wire.beginTransmission(VCNL4040_ADDR);
  Wire.write(addr);
  Wire.write(val);
  Wire.endTransmission(); //Release bus
}

//Write a two byte value to a Command Register
void writeToCommandRegister(byte addr, byte lowVal, byte highVal)
{
  Wire.beginTransmission(VCNL4040_ADDR);
  Wire.write(addr);
  Wire.write((byte)lowVal); //Low byte of command
  Wire.write((byte)(highVal>>8)); //High byte of command
  Wire.endTransmission(); //Release bus
}
