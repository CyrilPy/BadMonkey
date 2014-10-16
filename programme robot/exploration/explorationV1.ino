// code qui parcours la pièce
// How to use it :
// - open the Arduino serial console

#include <Arduino.h>
#include <Console.h>

#include <YunServer.h>
#include <YunClient.h>

#include <Wire.h>

#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

#define DEFAULT_STEP_NUMBER		100

#define INFRARED_SENSOR_INPUT	A0

#define TOOCLOSE 75

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor * motor1 = AFMS.getStepper(200, 2);
Adafruit_StepperMotor * motor2 = AFMS.getStepper(200, 1);

YunServer server;

void motorInitialize()
{

	AFMS.begin();
	
	// rotation per min
	motor1->setSpeed(60); 
  	motor2->setSpeed(60);
  	motor1->release();
  	motor2->release(); 


}

void motorForward(int stepReq = 1)
{
	for (int i=0; i < DEFAULT_STEP_NUMBER; i++)
	{
		motor1->step(stepReq, FORWARD, SINGLE);
		motor2->step(stepReq, FORWARD, SINGLE); 
	}
}

void motorBackward(int stepReq = 1)
{
	for (int i=0; i < DEFAULT_STEP_NUMBER; i++)
	{
		motor1->step(stepReq, BACKWARD, SINGLE); 
		motor2->step(stepReq, BACKWARD, SINGLE); 
	}
}

void motorTurnLeft(int stepReq = 1)
{
	for (int i=0; i < DEFAULT_STEP_NUMBER; i++)
	{
		motor1->step(stepReq, BACKWARD, SINGLE); 
		motor2->step(stepReq, FORWARD, SINGLE); 
	}
}


void motorTurnRight(int stepReq = 1)
{
	for (int i=0; i < DEFAULT_STEP_NUMBER; i++)
	{
		motor1->step(stepReq, FORWARD, SINGLE); 
		motor2->step(stepReq, BACKWARD, SINGLE); 
	}
}

void setup()
{
	Bridge.begin();
  	Console.begin();

	while (!Console)
  	{
  		// wait Arduino Console connection.
  	}

	Console.println("Simple loop for infrared sensor !");

	pinMode(INFRARED_SENSOR_INPUT, INPUT);
        motorInitialize();
}


void loop()
{
	int analogValue = analogRead(INFRARED_SENSOR_INPUT);
        motorForward(20);
        motorTurnLeft(92);

	Console.println(analogValue);
        if(analogValue <= TOOCLOSE)
        {
          motorTurnRight(46);
          
        }
        else 
          motorTurnLeft(46);
	
	delay(50);
}

