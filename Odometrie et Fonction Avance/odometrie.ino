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

//Robot parameters
#define WHEEL_DIAM  69 // mm
#define WHEEL_PERIM  218 // mm
#define STEPMOTOR  200  // pas du moteur pour faire 360°
#define RADIAN_PER_DEG  3.14159265359/180 
#define TRACK  123  // Entraxe entre le mileu des roues
#define ENCODER_FACTOR  WHEEL_PERIM/20 
#define COEFF_CORRECTION 3 //Coefficient correcteur pour le frottement

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor * motor1 = AFMS.getStepper(200, 2);
Adafruit_StepperMotor * motor2 = AFMS.getStepper(200, 1);

bool run;
int counter;

//Parametres pour l'odometrie
int left_encoder;
int right_encoder;
float dd ;
float dg ;
float theta;
float wg;
float wd;
float xPrime;
float yPrime;
float posX ;
float posY ;


//Initialisation des moteurs
void motorInitialize()
{
	run = false;

	AFMS.begin();
	
	// rotation per min
	motor1->setSpeed(100); 
  	motor2->setSpeed(100);
  	motor1->release();
  	motor2->release(); 
}

//Marche avant
void motorForward()
{
	for (int i=0; i < 275; i++)
	{
		motor1->step(1, FORWARD, SINGLE);
		motor2->step(1, FORWARD, SINGLE); 
	}
        calculPosition (DEFAULT_STEP_NUMBER,DEFAULT_STEP_NUMBER);
}

//Marche avant de X mm
void motorForward(int lenght_to_do)
{
    int steptodo = (double)lenght_to_do / (double)WHEEL_PERIM * (double) 200;
	Console.println("motorForward");
        Console.println(steptodo);
        for (int i=0; i < steptodo ; i++)
	{
		motor1->step(1, FORWARD, SINGLE);
		motor2->step(1, FORWARD, SINGLE); 
	}
        calculPosition (DEFAULT_STEP_NUMBER,DEFAULT_STEP_NUMBER);
}


//Marche arriere de X mm
void motorBackward(int lenght_to_do)
{
        int steptodo = (double)lenght_to_do / (double)WHEEL_PERIM * (double) 200;
	for (int i=0; i < steptodo; i++)
	{
		motor1->step(1, BACKWARD, SINGLE); 
		motor2->step(1, BACKWARD, SINGLE); 
	}
        calculPosition (-DEFAULT_STEP_NUMBER,-DEFAULT_STEP_NUMBER);
}

//Marche arriere PAR DEFAUT
void motorBackward()
{       
	for (int i=0; i < DEFAULT_STEP_NUMBER; i++)
	{
		motor1->step(1, BACKWARD, SINGLE); 
		motor2->step(1, BACKWARD, SINGLE); 
	}
        calculPosition (-DEFAULT_STEP_NUMBER,-DEFAULT_STEP_NUMBER);
}


//Calcul la position du robot (Odometrie)
void calculPosition (int step_left, int step_right)
{
        left_encoder = step_left;
        right_encoder = step_right;
                
        //Calcul des deplacements en millimetres
        float dd = right_encoder * ENCODER_FACTOR;
        float dg = left_encoder * ENCODER_FACTOR;
                
        //Calcul de l'angle theta
        theta = (dd-dg)/TRACK ;
                
        //Calcul des vitesses angulaires
        wg = left_encoder * 1.8 * RADIAN_PER_DEG;
        wg = wg/3;
        wd = right_encoder * 1.8 * RADIAN_PER_DEG;
        wd = wd/3;
                
        //Calcul des composantes vitesses
        xPrime = (WHEEL_DIAM/2) * ( (wd+wg)/2 * cos(theta) );
        yPrime = (WHEEL_DIAM/2) * ( (wd+wg)/2 * sin(theta) );
        
        posX = posX + ( 3 * xPrime);
        posY = posY + ( 3 * yPrime);
        
        //Affichage de la console
        Console.println("Encodeur gauche: ");
        Console.println("Encodeur droit: ");
        Console.println(dd);
        Console.println(dg);
        Console.println(theta);
        Console.println(wg);
        Console.println(wd);
        Console.println(xPrime);
        Console.println(yPrime);
        Console.println("Position :");
        Console.println(posX);
        Console.println(posY);
}


//Convertie des degres en step moteur
int calcul_step_turn(int degree_to_turn){

    float x = ((double)degree_to_turn) /((double) 360);
    float distance_for_turn = x * PI * TRACK;
    int step_to_turn = COEFF_CORRECTION + distance_for_turn * STEPMOTOR / WHEEL_PERIM;
   
    Console.println("steptoturn:");
    Console.println(step_to_turn);
    return step_to_turn;
}

//Fonction tourne a gauche de X degres
void turnDegreeLeft(int degree_to_turn){
    Console.println("turnDegreeLeft");
    Console.println(degree_to_turn);
    Console.println("turnleft");
    int steptodo = calcul_step_turn(degree_to_turn);
    Console.println("steptodo:"+steptodo);
	for (int i=0; i < steptodo; i++)
	{
		motor1->step(1, FORWARD, SINGLE); 
		motor2->step(1, BACKWARD, SINGLE); 
	}
        calculPosition (steptodo,-steptodo);
}

//Fonction tourne a droite de X degres
void turnDegreeRight(int degree_to_turn){
  
    float distance_for_turn = degree_to_turn/360 * PI * TRACK;
   
    int steptodo = COEFF_CORRECTION + distance_for_turn * STEPMOTOR / WHEEL_PERIM;
   
    for (int i=0; i < steptodo; i++)
	{
		motor1->step(1, FORWARD, SINGLE); 
		motor2->step(1, BACKWARD, SINGLE); 
	}
        calculPosition (steptodo,-steptodo);
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
                                Console.print("salut");
				Console.print("Analog value: ");
				Console.println(analogValue);
			break;
			//--------- move forward
			case 'f':
				Console.println("Move forward...");
				motorForward(500);
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
			//--------- move right
			case 'g':
				Console.println("Motor Turn Left ...");
				 turnDegreeLeft(90);
                                 break;
                        //--------- move right
			case 'r':           
				Console.println("Motor Turn Right ...");	
			break;
		}
	}

	if (run==true)
	{
		counter++;

		motorForward();
                

		Console.println("I'm alive !");

		if (counter==50)
		{
			run = false;
			Console.println("stop counter");
		}
	}
}

