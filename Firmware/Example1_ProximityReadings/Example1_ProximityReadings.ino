/*
   Robotic Finger Sensor - VCNL4040 basic test code example
   By: Rob Reynolds
   SparkFun Electronics
   Date: February 14th, 2017
   License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware License).

   Outputs the IR Proximity Value from the VCNL4040 sensor.

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
  Serial.begin(9600);
  Serial.println("Hello, world!");

  Wire.begin(); //Join i2c bus

  int deviceID = readFromCommandRegister(ID);
  if (deviceID != 0x186)
  {
    Serial.println("Device not found. Check wiring.");
    Serial.print("Expected: 0x186. Heard: 0x");
    Serial.println(deviceID, HEX);
    while (1); //Freeze!
  }
  Serial.println("VCNL4040 detected!");

  initVCNL4040(); //Configure sensor

  for (int x = 0 ; x < 0x07 ; x++)
  {
    Serial.print("Register 0x");
    Serial.print(x, HEX);
    Serial.print(": 0x");
    Serial.print(readFromCommandRegister(x), HEX);
    Serial.println();
  }
  Serial.println();
  
}

void loop()
{
  unsigned int distance = readFromCommandRegister(PS_DATA_L); //Get proximity values

  Serial.print("distance: ");
  Serial.print(distance);
  Serial.println();

  delay(100);
}

//Configure the various parts of the sensor
void initVCNL4040()
{
  //Clear PS_SD to turn on proximity sensing
  //byte conf1 = 0b00000000; //Clear PS_SD bit to begin reading
  byte conf1 = 0b00001110; //Integrate 8T, Clear PS_SD bit to begin reading
  byte conf2 = 0b00001000; //Set PS to 16-bit
  //byte conf2 = 0b00000000; //Clear PS to 12-bit
  writeToCommandRegister(PS_CONF1, conf1, conf2); //Command register, low byte, high byte

  //Set the options for PS_CONF3 and PS_MS bytes
  byte conf3 = 0x00;
  //byte ms = 0b00000010; //Set IR LED current to 100mA
  //byte ms = 0b00000110; //Set IR LED current to 180mA
  byte ms = 0b00000111; //Set IR LED current to 200mA
  writeToCommandRegister(PS_CONF3, conf3, ms);
}

//Reads a two byte value from a command register
unsigned int readFromCommandRegister(byte commandCode)
{
  Wire.beginTransmission(VCNL4040_ADDR);
  Wire.write(commandCode);
  Wire.endTransmission(false); //Send a restart command. Do not release bus.

  Wire.requestFrom(VCNL4040_ADDR, 2); //Command codes have two bytes stored in them

    uint8_t lsb = Wire.read();
    uint8_t msb = Wire.read();
    return ((uint16_t)msb << 8 | lsb);

  unsigned int data = Wire.read();
  data |= Wire.read() << 8;

  return (data);
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
void writeToCommandRegister(byte commandCode, byte lowVal, byte highVal)
{
  Wire.beginTransmission(VCNL4040_ADDR);
  Wire.write(commandCode);
  Wire.write(lowVal); //Low byte of command
  Wire.write(highVal); //High byte of command
  Wire.endTransmission(); //Release bus
}
