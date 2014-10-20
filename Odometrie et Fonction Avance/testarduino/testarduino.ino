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
#define WHEEL_DIAM  69     // mm
#define WHEEL_PERIM  218   // mm
#define STEPMOTOR  200     // pas du moteur pour faire 360°
#define RADIAN_PER_DEG  3.1415/180 
#define TRACK  123  // Entraxe entre le mileu des roues
#define ENCODER_FACTOR  WHEEL_PERIM/200 
#define COEFF_CORRECTION 3 //Coefficient correcteur pour le frottement

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor * motor1 = AFMS.getStepper(200, 2);
Adafruit_StepperMotor * motor2 = AFMS.getStepper(200, 1);

bool run;
int counter;

//Parametres pour l'odometrie
unsigned int  traveled_distance;  //Distance totale parcourue
float theta = 0;      // Orientation du robot
float posRob[2];      // Coordonées du robot
float posObj [2];  // Coordoonées d'un obstacle


 

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
    int steptodo = (double)lenght_to_do / (double)WHEEL_PERIM * (double) 200 + 1;
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
        int steptodo = (double)lenght_to_do / (double)WHEEL_PERIM * (double) 200 + 1;
	for (int i=0; i < steptodo; i++)
	{
		motor1->step(1, BACKWARD, SINGLE); 
		motor2->step(1, BACKWARD, SINGLE); 
	}
        calculPosition (-steptodo,-steptodo);
}

//Marche arriere par DEFAUT
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
        // 1 Calcul de la distance parcouru par chaque roue 
        float distance_wheel_left  =  (float) step_left * (float)WHEEL_PERIM / (float)STEPMOTOR;
        float distance_wheel_right = (float) step_right * (float)WHEEL_PERIM /  (float)STEPMOTOR;
        float distance = (distance_wheel_left + distance_wheel_left)/2;
        
        traveled_distance= traveled_distance + abs(distance)/10;

        //2 Conversion de l'angle theta en radian
        float thetaRadian = theta * PI / 180;
        
        //3 Calcul de la position
        float yPrime = distance * cos(thetaRadian);
        float xPrime = distance * sin(thetaRadian);
        
        //4 Incrementation avec les positions précedentes
        posRob[0] = posRob[0] + xPrime;
        posRob[1] = posRob[1] + yPrime;
        
        
}



//Convertie des degres en step moteur
int calcul_step_turn(int degree_to_turn){
    
    float x = ((float)degree_to_turn) /((float) 360);
    float distance_for_turn = x * PI * (float) TRACK;
    int step_to_turn = 1 + distance_for_turn * STEPMOTOR / WHEEL_PERIM;

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
        theta = (theta + degree_to_turn) % 360;
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
        theta = (theta - degree_to_turn) % 360;

}


//Fonction qui calcul la position d'un point d'un obstacle
void localizePointObject (float irDetection){
    
     //1 Calcul de X, Y du point detecté par rapport au robot
     float posObjX = irDetection * sin (theta);
     float posObjY = irDetection * cos (theta);
     
     //2 Translation des coordonées par rapport au repère base
     //Enregistrement dans le tableau
     posObj [0] =  posRob[0] + posObjX;
     posObj [1] =  posRob[1] + posObjY;
    
}

//Enregistre la valeur de l'infra-rouge
float recordIR()
{
  int cumulAnalogValue = 0;
  int analogValue;
  int samples = 20;
  for (int i = 1; i <= samples; i++)
  {
    analogValue = analogRead(INFRARED_SENSOR_INPUT);
    cumulAnalogValue += analogValue; 
    Console.println("Instant value :");
    Console.println(analogValue);
    delay(20);   
  }
  Console.println("Mean value :");
  Console.println(cumulAnalogValue/samples);
           
  double distance = (double)cumulAnalogValue/samples;
  //Polynome : 3,09806E-14	-7,88745E-11	8,10272E-08	-4,30409E-05	0,012581264	-1,991097159	156,2339364 
  //Polynom V2 : 2,76075E-14	-6,99111E-11	7,15462E-08	-3,80057E-05	0,011194502	-1,807762943	146,981348             
  double distanceCM;
  distanceCM = 
    pow(distance, 6)*(2.76075/100000000000000) +
    pow(distance, 5)*(-6.99111/100000000000) +
    pow(distance, 4)*(7.15462/100000000) +
    pow(distance, 3)*(-3.80057/100000) +
    pow(distance, 2)*0.011194502 +
    (distance*(-1.807762943)) +
    146.981348;
  Console.println(distanceCM * 10);	
  return (double)distanceCM * 10;
   
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
                        //--------- test 
                        case 't':           
				Console.println("---------- TEST ----------");
                                motorForward(500);
                                turnDegreeRight(45);
                                localizePointObject(recordIR());
                                	
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

