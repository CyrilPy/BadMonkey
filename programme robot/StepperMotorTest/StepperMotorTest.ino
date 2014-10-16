// Test of stepper motor on the Arduino Yun.
// How to use it :
// - open the Arduino serial console

#include <Wire.h>
#include <Arduino.h>
#include <Console.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"


#define DEFAULT_STEP_NUMBER		100

#define INFRARED_SENSOR_INPUT	A0


Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor * motor1 = AFMS.getStepper(200, 2);
Adafruit_StepperMotor * motor2 = AFMS.getStepper(200, 1);

bool run ;
int counter;


void motorInitialize()
{
	run = false;

	AFMS.begin();
	
	// rotation per min
	motor1->setSpeed(60); 
  	motor2->setSpeed(60);
  	motor1->release();
  	motor2->release(); 


}

void motorForward()
{
	for (int i=0; i < DEFAULT_STEP_NUMBER; i++)
	{
		motor1->step(1, FORWARD, SINGLE);
		motor2->step(1, FORWARD, SINGLE); 
	}
}

void motorBackward()
{
	for (int i=0; i < DEFAULT_STEP_NUMBER; i++)
	{
		motor1->step(1, BACKWARD, SINGLE); 
		motor2->step(1, BACKWARD, SINGLE); 
	}
}

void motorTurnLeft()
{
	for (int i=0; i < DEFAULT_STEP_NUMBER; i++)
	{
		motor1->step(1, BACKWARD, SINGLE); 
		motor2->step(1, FORWARD, SINGLE); 
	}
}


void motorTurnRight()
{
	for (int i=0; i < DEFAULT_STEP_NUMBER; i++)
	{
		motor1->step(1, FORWARD, SINGLE); 
		motor2->step(1, BACKWARD, SINGLE); 
	}
}


void setup()
{
	Bridge.begin();
  	Console.begin();

//	while (!Console)
//  	{
//  		// wait Arduino Console connection.
//  	}

	Console.println("Test of Stepper Motor !");

	motorInitialize();

	pinMode(INFRARED_SENSOR_INPUT, INPUT);

}

void loop()
{
	int analogValue;

	int command = Console.read();
	if (command!=-1)
	{
		switch(command)
		{
			//--------- read analog sensor
			case 'a':
				analogValue = analogRead(INFRARED_SENSOR_INPUT);
				Console.print("Analog value: ");
				Console.println(analogValue);
			break;
			//--------- move forward
			case 'f':
				Console.println("Move forward...");
				motorForward();
			break;
			//--------- move backward
			case 'b':
				Console.println("Move backward...");
				motorBackward();
			break;
			//--------- motor 1, one step
			case '1':
				Console.println("single motor1");
				motor1->step(1, BACKWARD, SINGLE); 
				break;
			//--------- motor 1 and 2, one step
			case '2':
				Console.println("single motor1 2");
				motor1->step(1, BACKWARD, SINGLE); 
				delay(1);
				motor2->step(1, BACKWARD, SINGLE);
				break;
			//--------- movement sequence test
			case 's':
				if (run==false)
				{
					Console.println("run");
					run = true;
					counter = 0;
				}
			break;
		}
	}

	if (run==true)
	{
		counter++;

		motorForward();
		motorTurnRight();
		motorTurnLeft();
		motorBackward();

		Console.println("I'm alive !");

		if (counter==50)
		{
			run = false;
			Console.println("stop counter");
		}
	}
}
