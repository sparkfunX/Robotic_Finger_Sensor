//VNCL4040 Register addresses
#define COMMAND_0 0x80  // starts measurements, relays data ready info
#define PRODUCT_ID 0x81  // product ID/revision ID, should read 0x21
#define IR_CURRENT 0x83  // sets IR current in steps of 10mA 0-200mA
#define AMBIENT_PARAMETER 0x84  // Configures ambient light measures
#define PROXIMITY_MOD 0x8F  // proximity modulator timing

#define VCNL4040_ADDR 0x60 //7-bit unshifted I2C address of VCNL4040

//Command Registers have an upper byte and lower byte.
#define PS_CONF1 0x03
//#define PS_CONF2 //High byte of PS_CONF1
#define PS_CONF3 0x04
//#define PS_MS //High byte of PS_CONF3
#define PS_DATA_L 0x08
//#define PS_DATA_M //High byte of PS_DATA_L

#define ID  0x0C //Register that contains the VCNL4040's static ID

//Get the raw proximity/distance value from the sensor
unsigned int readProximity()
{
  return(readFromCommandRegister(PS_DATA_L));
}

//Check to see if VCNL4040 responds
boolean testVCNL4040()
{
  int deviceID = readFromCommandRegister(ID);
  if (deviceID != 0x186)
  {
    return(false);
  }

  return(true);
  Serial.println("VCNL4040 detected!");  
}

//Configure the various parts of the sensor
void initVCNL4040()
{
  //Clear PS_SD to turn on proximity sensing
  writeToCommandRegister(PS_CONF1, 0x00, 0x00); //Command register, low byte, high byte

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

  while (!Wire.available()) delay(1);

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
  Wire.write((byte)(highVal >> 8)); //High byte of command
  Wire.endTransmission(); //Release bus
}
