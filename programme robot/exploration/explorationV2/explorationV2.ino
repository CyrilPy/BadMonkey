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

String etatHttp;
String etatParcours;

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
void parcoursMainGauche()
{
  
    float distanceMin=90;
    float lastDistance=0;
    short int angleDistMin=0;
    switch (etatParcours)
    {
    //recherche du mur le plus proche
      case 1:
        for(int angle=0; angle<=360; angle+=10)
        {
          lastDistance=getDistance()
          if ( lastDistance < distanceMin)
          {
            distanceMin=lastDistance;
            angleDistMin=angle;
          }
          turnLeftDegree(10);
        }
        turnLeftDegree(angleDistMin);
        etatParcours=2;
        break;
        
        
      case 2:
        //verifier si l'on a bien le point le plus proche a +/-10°
        turnRightDegree(10);
        for(int angle=0,angle<=20,angle++)
        {
          lastDistance=getDistance()
          if ( lastDistance < distanceMin)
          {
            distanceMin=lastDistance;
            angleDistMin=angle;
          }
          turnLeftDegree(angle);
        }
        turnRightDistance(angleDistMin);
        etatPArcours = 3;
        break;
        
      case 3:
        //etat ou l'on avance tout droit jusqu'à être a 7cm du mur
        lastDistance=getDistance();
        do
        {
          motorForward();
          lastDistance=getDistance();
        }while(lastDistance > 7);
        etatParcours=4;
        break;
        
       //tourné a droite
       case 4:
         motorDegreeRight(90);
         etatParcours = 5;
         break;
         
        case 5:
          //regarder s'il y a un mur en face
          lastDistance=getDistance();
          if(lastDistance > 7 && lastDistance < 10)
          {  
            //tournée à droite
             etatParcours=4;
             break;
          }
          else
          {
             etatParcours=6;
             break;
          }
          
        case 6:
        //avancer tous droit
        moveForward(20);
        etatsParcours=7;
        break;
        
        //tourné a gauche
       case 7:
         motorDegreeLeft(90);
         etatParcours = 5;
         break;
            
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

