// Test of stepper motor on the Arduino Yun.
// How to use it :
// - open the Arduino serial console

#include <Wire.h>
#include <Arduino.h>
#include <Console.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"


#define DEFAULT_STEP_NUMBER	100
#define INFRARED_SENSOR_INPUT	A0

//Robot parameters
#define WHEEL_DIAM  69 // mm
#define WHEEL_PERIM  218 // mm
#define STEPMOTOR  200  // pas du moteur pour faire 360°
#define RADIAN_PER_DEG  3.14159265359/180 
#define TRACK  123  // Entraxe entre le mileu des roues
#define ENCODER_FACTOR  WHEEL_PERIM/200 
#define COEFF_CORRECTION 3 //Coefficient correcteur pour le frottement

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor * motor1 = AFMS.getStepper(200, 2);
Adafruit_StepperMotor * motor2 = AFMS.getStepper(200, 1);

bool run;
int counter;

//Parametres pour l'odometrie
int encoder_left_saved;
int encoder_right_saved;
int encoder_left;
int encoder_right;

float theta = 0;
float posX =0;
float posY =0;


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
        calculPosition (steptodo,steptodo);
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
        calculPosition (-steptodo,-steptodo);
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
         
        Console.println("----------Calcul Position--------- ");
        //Sauvegarde du compteur des roues
        encoder_left_saved = encoder_left_saved + step_left;
        encoder_right_saved = encoder_right_saved + step_right;
        
        Console.println("encoder_left_saved: ");
        Console.println(encoder_left_saved);
        Console.println("encoder_right_saved: ");
        Console.println(encoder_right_saved);
        
        // 1 Calcul de la distance parcouru par chaque roue 
        float distance_wheel_left  =  (float) step_left * (float)WHEEL_PERIM / (float)STEPMOTOR;
        float distance_wheel_right = (float) step_right * (float)WHEEL_PERIM /  (float)STEPMOTOR;
        float distance = (distance_wheel_left + distance_wheel_left)/2;
        
        Console.println("distance_wheel_left: ");
        Console.println(distance_wheel_left);
        Console.println("distance_wheel_right: ");
        Console.println(distance_wheel_right);
         
        //2 Conversion de l'angle theta en radian
        float thetaRadian = theta * PI / 180;
        
        Console.println("thetaRadian: ");
        Console.println(thetaRadian);
         
        //3 Calcul de la position
        float yPrime = distance * cos(thetaRadian);
        float xPrime = distance * sin(thetaRadian);
        
        Console.println("yPrime: ");
        Console.println(yPrime);
        Console.println("xPrime: ");
        Console.println(xPrime);
        
        //Incrementation avec les positions précedentes
        posY = posY + yPrime;
        posX = posX + xPrime;
        
        Console.println("posY: ");
        Console.println(posY);
        Console.println("posX: ");
        Console.println(posX);
        
}



//Convertie des degres en step moteur
int calcul_step_turn(int degree_to_turn){

    float x = ((float)degree_to_turn) /((float) 360);
    float distance_for_turn = x * PI * (float) TRACK;
    int step_to_turn = 1 + distance_for_turn * STEPMOTOR / WHEEL_PERIM;
   
   
    Console.println("----------Calcul Step Turn ----------");
    Console.println("step_to_turn:");
    Console.println(step_to_turn);
    return step_to_turn;
}

//Fonction tourne a gauche de X degres
void turnDegreeLeft(int degree_to_turn){
  
    int steptodo = calcul_step_turn(degree_to_turn);
	for (int i=0; i < steptodo; i++)
	{
		motor1->step(1, FORWARD, SINGLE); 
		motor2->step(1, BACKWARD, SINGLE); 
	}
        
        //Enregistrement du theta
        theta = theta + degree_to_turn;
        Console.println("thetha:");
        Console.println(theta);
}

//Fonction tourne a droite de X degres
void turnDegreeRight(int degree_to_turn){
  
    int steptodo = calcul_step_turn(degree_to_turn);
    for (int i=0; i < steptodo; i++)
	{
		motor1->step(1, BACKWARD, SINGLE); 
		motor2->step(1, FORWARD, SINGLE); 
	}
        
        //Enregistrement du theta
        theta = theta - degree_to_turn;
        Console.println("thetha:");
        Console.println(theta);
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
				motorBackward(500);
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
				 turnDegreeLeft(45);
                                 break;
                        //--------- move right
			case 'r':           
				Console.println("Motor Turn Right ...");
                                turnDegreeRight(45);	
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

