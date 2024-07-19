// Add Comment to Disable Alternate XBOX Receiver Mode
#define XBOX_ALT_MODE

#include <Arduino.h>
#include "CytronMotorDriver.h"
#include <math.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#ifdef XBOX_ALT_MODE
#include <XBOXUSB.h>
#else
#include <XBOXRECV.h>
#endif

// Define the deadzone and controller id
#define conid 0
#define deadzone 0

// Define the minimum and maximum speed
#define minSpeed 10
#define maxSpeed 255

// Define the PWM and DIR pins for the motors
#define pwm1 3
#define pwm2 2
#define pwm3 5
#define pwm4 4
#define pwm5 6
#define pwm6 7
#define dir1 22
#define dir2 24
#define dir3 4
#define dir4 47
#define dir5 38
#define dir6 39

// Create the motor objects
CytronMD motor1(PWM_DIR, pwm1, dir1);
CytronMD motor2(PWM_DIR, pwm2, dir2);
CytronMD motor3(PWM_DIR, pwm3, dir3);
CytronMD motor4(PWM_DIR, pwm4, dir4);
CytronMD motor5(PWM_DIR, pwm5, dir5);
CytronMD motor6(PWM_DIR, pwm6, dir6);

// Create the USB object
USB Usb;
#ifdef XBOX_ALT_MODE
XBOXUSB Xbox(&Usb);
#else
XBOXRECV Xbox(&Usb);
#endif

// Define Functions
bool xboxConnCheck()
{
#ifdef XBOX_ALT_MODE
  if (Xbox.Xbox360Connected)
  {
    return true;
  }
  return false;
#else
  if (Xbox.XboxReceiverConnected)
  {
    if (Xbox.Xbox360Connected[conid])
    {
      return true;
    }
  }
  return false;
#endif
}

int left = 0, right = 0;

void updateMotors(int YSpeed, int TSpeed)
{
  left = constrain(YSpeed - TSpeed, -255, 255);
  right = constrain(YSpeed + TSpeed, -255, 255);
  Serial.print(left);
  Serial.println(right);
  motor1.setSpeed(left);
  motor2.setSpeed(right);
  motor3.setSpeed(-left);
  motor4.setSpeed(right);
  motor5.setSpeed(left);
  motor6.setSpeed(right);
}

// Setup Function
void setup()
{
  Wire.begin();
  Serial.begin(9600);
#if !defined(_MIPSEL_)
  while (!Serial)
    ;
#endif
  if (Usb.Init() == -1)
  {
    Serial.print(F("\r\nOSC did not start"));
    while (1)
      ;
  }
  Serial.print(F("\r\nXbox Wireless Receiver Library Started"));

  updateMotors(0, 0);
}

// Loop Function
void loop()
{
  Usb.Task();
  if (xboxConnCheck())
  {
#ifdef XBOX_ALT_MODE
    int leftHatY = Xbox.getAnalogHat(LeftHatY);
    int rightHatX = Xbox.getAnalogHat(RightHatX);
#else
    int leftHatY = Xbox.getAnalogHat(LeftHatY, conid);
    int rightHatX = Xbox.getAnalogHat(RightHatX, conid);
#endif

    int YSpeed = 0, TSpeed = 0;

    if (leftHatY > deadzone)
    {
      YSpeed = map(leftHatY, deadzone, 32767, minSpeed, maxSpeed);
    }
    else if (leftHatY < -deadzone)
    {
      YSpeed = map(leftHatY, -32768, -deadzone, -maxSpeed, -minSpeed);
    }
    if (rightHatX > deadzone)
    {
      TSpeed = map(rightHatX, deadzone, 32767, minSpeed, maxSpeed);
    }
    else if (rightHatX < -deadzone)
    {
      TSpeed = map(rightHatX, -32768, -deadzone, -maxSpeed, -minSpeed);
    }
    updateMotors(YSpeed, TSpeed);
  }
  else
  {
    updateMotors(0, 0);
  }
}