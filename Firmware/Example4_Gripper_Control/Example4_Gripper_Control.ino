/*
  Robotic Finger Sensor - Sensing a touch and controlling a gripper
  By: Nathan Seidle
  SparkFun Electronics
  Date: March 14th, 2017
  License: This software is open source and can be used for any purpose.

  This code identifies touch/release events and controls (stops) a servo to create a robotic gripper with light grip,
  on most any sized/shaped object.

  Open a terminal window at 115200 and view the state as you touch the sensor.
  Press 'z' or 'c' to begin closing the servo. Stops (plus a small grip amount) when touch is detected.

  This code was designed to work with an Arduino Uno but should work nicely with Teensy and others.

  Brought to you by SparkFun (orignial code), the Correll Lab at the University
  of Colorado, Boulder and Robotic Materials Inc.
*/

/***** Library parameters ****/
#include <Servo.h>
Servo gripperServo;

#define WIRE Wire //For teensyLC and Arduino Uno
//#define WIRE Wire1 //For others

#include <Wire.h> //For Arduino Uno
//#include <i2c_t3.h> //Use <i2c_t3.h> for Teensy

#include <math.h>

const byte servo = 9; //Attach servo PWM pin here. Any PWM pin is ok.
const byte button = 7; //Press the button, cause gripper to close or open

#define LOOP_TIME 10  // loop duration in ms

// Touch/release detection
#define EA 0.3  // exponential average weight parameter / cut-off frequency for high-pass filter

/***** GLOBAL VARIABLES *****/
unsigned int proximity_value; // current proximity reading
unsigned int average_value;   // low-pass filtered proximity reading
signed int fa2;              // FA-II value;
signed int sensitivity = 100;  // Sensitivity of touch/release detection, values closer to zero increase sensitivity

boolean movement = true; //Keeps track of if we are sensing movement or not
byte noMovementCounts = 0; //Keeps track of no movement detected from sensor

byte gripperState = 0; //Statemachine variable
#define STATE_OPEN 0
#define STATE_CLOSED 1
#define STATE_OPENING 2
#define STATE_CLOSING 3
#define STATE_STOPPED 4
#define STATE_CONTACT 5

byte touchState = 0; //Statemachine variable
#define NOT_TOUCHING 0
#define TOUCHING 1

int servoPosition = 150; //Keeps track of where the servo is currently at
const byte servoOpenPosition = 150; //Max open gripper position
const byte servoClosedPosition = 65; //Min closed gripper position
const byte closingRate = 1; //Rate at which to close the servo. Setting too high risks servo putting too much pressure before sensing touch
const byte openingRate = 100; //Rate at which to open the servo. Can be large.
const byte gripAmount = 5; //Additional servo position from contact point to get grip

void setup()
{
  Serial.begin(115200);

  WIRE.begin();

  pinMode(button, INPUT_PULLUP);

  //Setup servo
  gripperServo.attach(servo);
  gripperServo.write(servoOpenPosition); //Goto the resting position (gripper open)
  gripperState = STATE_OPEN;
  servoPosition = servoOpenPosition;
  touchState = NOT_TOUCHING;

  delay(100);

  if (testVCNL4040() == false)
  {
    Serial.println("Device not found. Check wiring.");
    while (1); //Freeze!
  }
  Serial.println("VCNL4040 detected!");

  initVCNL4040(); //Configure sensor

  delay(10);
  average_value = readProximity(); //Get proximity values
  fa2 = 0;
}

void loop()
{
  unsigned long startTime = millis();

  if(digitalRead(button) == LOW)
  {
    delay(50); //Debounce
    while(digitalRead(button) == LOW) delay(1); //Wait for user to stop pressing button
    
    if (touchState == TOUCHING) gripperState = STATE_OPENING;
    else if(gripperState == STATE_CLOSING) gripperState = STATE_OPENING;
    else if(gripperState == STATE_OPEN) gripperState = STATE_CLOSING;
    else if(gripperState == STATE_CLOSED) gripperState = STATE_OPENING;
    else if(gripperState == STATE_OPENING && touchState != TOUCHING) gripperState = STATE_CLOSING;
    else gripperState = STATE_OPENING;
  }

  if (Serial.available())
  {
    byte incoming = Serial.read();
    if (incoming == 'c' || incoming == 'z')
    {
      //Don't keep closing if we're touching
      if (touchState != TOUCHING) gripperState = STATE_CLOSING;
    }
    else if (incoming == 'o' || incoming == 'a')
    {
      gripperState = STATE_OPENING;
    }
  }

  proximity_value = readProximity(); //Get proximity values
  fa2 = (signed int) average_value - proximity_value;

  //Serial.print(proximity_value);
  //Serial.print(",");
  //Serial.print(fa2);
  //Serial.print(",");

  if (fa2 > sensitivity && movement == false)
  {
    movement = true;
    noMovementCounts = 0;
    Serial.println("R");
    touchState = NOT_TOUCHING;

    //If needed, put code here to re-close the gripper to maintain grip
  }
  else if (fa2 < -sensitivity && movement == false)
  {
    if (gripperState == STATE_CLOSING)
    {
      //Contact! Continue closing for some grip
      gripperState = STATE_CONTACT;
    }

    movement = true;
    noMovementCounts = 0;
    Serial.println("T");
    touchState = TOUCHING;
  }

  //See if there is no movement detected
  if (abs(fa2) < 5)
  {
    noMovementCounts++;
    if (noMovementCounts > 2) movement = false;
    if (noMovementCounts > 100) noMovementCounts = 100; //Limit variable so no overflow
  }

  //See if we have a proximity that is too large, then we aren't touching
  if(proximity_value < 200)
  {
    touchState = NOT_TOUCHING;
  }

  if (gripperState == STATE_CLOSING)
  {
    Serial.println("Closing");
    servoPosition -= closingRate;
    if (servoPosition <= servoClosedPosition)
    {
      gripperState = STATE_CLOSED;
      servoPosition = servoClosedPosition;
    }

    gripperServo.write(servoPosition); //Adjust the servo closed
  }
  else if (gripperState == STATE_OPENING)
  {
    Serial.println("Opening");
    servoPosition += openingRate;
    if (servoPosition >= servoOpenPosition)
    {
      gripperState = STATE_OPEN;
      servoPosition = servoOpenPosition;
    }

    gripperServo.write(servoPosition); //Adjust the servo open
  }
  else if (gripperState == STATE_CONTACT)
  {
    Serial.println("Contact!");
    servoPosition -= gripAmount; //Move servo a little more to get grip
    gripperServo.write(servoPosition); //Adjust the servo open
    delay(10); //Wait for servo to get there

    gripperState = STATE_STOPPED;
  }

  //Print all the things
  Serial.print("ServoPosition: ");
  Serial.print(servoPosition);

  Serial.print(" State: ");
  if (gripperState == STATE_OPEN) Serial.print("Open");
  else if (gripperState == STATE_CLOSED) Serial.print("Closed");
  else if (gripperState == STATE_OPENING) Serial.print("Opening");
  else if (gripperState == STATE_CLOSING) Serial.print("Closing");
  else if (gripperState == STATE_STOPPED) Serial.print("Stopped");

  if (touchState == NOT_TOUCHING) Serial.print(" Not Touching");
  else if (touchState == TOUCHING) Serial.print(" Touching");

  Serial.print(" Prox: ");
  Serial.print(proximity_value);

  Serial.println();

  //Do this last
  average_value = EA * proximity_value + (1 - EA) * average_value;
  while (millis() < startTime + LOOP_TIME); // enforce constant loop time
}

