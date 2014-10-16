

#include <Arduino.h>
#include <Console.h>
#include <YunServer.h>
#include <YunClient.h>

#include <Wire.h>

#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"



#define DEFAULT_STEP_NUMBER		100

#define INFRARED_SENSOR_INPUT	A0

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor * motor1 = AFMS.getStepper(200, 2);
Adafruit_StepperMotor * motor2 = AFMS.getStepper(200, 1);

bool run ;
int counter;

YunServer server;

void motorInitialize()
{
	run = true;

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

void setup() {
  // put your setup code here, to run once:
        Bridge.begin();
  	Console.begin(); 


  	server.listenOnLocalhost();
  	server.begin();

  	Console.println("You're connected to the console !");
        motorInitialize();

	pinMode(INFRARED_SENSOR_INPUT, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
        YunClient client = server.accept();

	// Get command from the url: http://yourArduinoYun.local/arduino/toto
	if (client)
	{
        	executeUrlCommand(client);
        	client.stop();
  	}

	// Get command from the Console :
	if (Console.read()!=-1)
	{
		Console.println("Receive something...");
	}
}

void executeUrlCommand(YunClient client)
{
	String command = client.readStringUntil('/');

	Console.print("Execute ");
	Console.print(command);
	Console.print(" arg: ");

	if (command=="forward")
	{
		Console.println(client.readString());
                motorForward();
	}
	else if (command=="backward")
	{
		Console.println(client.readString());
                motorBackward();
	}
	else if (command=="turnLeft")
	{
		Console.println(client.readString());
                motorTurnLeft();
	}
        else if (command=="turnRight")
	{
		Console.println(client.readString());
                motorTurnRight();
	}
	else
	{
		Console.print("Huuu?? ");
		Console.println(client.readString());
	}

	// send answer to the client:
	client.print("Got your command : ");
	client.print(command);

}
