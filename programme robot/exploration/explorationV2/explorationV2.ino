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

//Robot parameters
#define WHEEL_DIAM  69 // mm
#define WHEEL_PERIM  218 // mm
#define STEPMOTOR  200  // pas du moteur pour faire 360°
#define RADIAN_PER_DEG  3.14159265359/180 
#define TRACK  123  // Entraxe entre le mileu des roues
#define ENCODER_FACTOR  WHEEL_PERIM/20 
#define COEFF_CORRECTION 1 //Coefficient correcteur pour le frottement

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor * motor1 = AFMS.getStepper(200, 2);
Adafruit_StepperMotor * motor2 = AFMS.getStepper(200, 1);

int counter;

//Parametres pour l'odometrie
int encoder_left_saved;
int encoder_right_saved;
int encoder_left;
int encoder_right;

float theta = 0;
float posX =0;
float posY =0;

YunServer server;

int etatHttp;
int etatParcours=1;  

void motorInitialize()
{

	
	AFMS.begin();
	
	// rotation per min
	motor1->setSpeed(100); 
  	motor2->setSpeed(100);  
  	motor1->release();
  	motor2->release(); 

}


int recordIR()
{
  int cumulAnalogValue = 0;
  int analogValue;
  int samples = 10;
  for (int i = 1; i <= samples; i++)
  {
    analogValue = analogRead(INFRARED_SENSOR_INPUT);
    cumulAnalogValue += analogValue; 
    //Console.println("Instant value :");
    //Console.println(analogValue);
    delay(10);   
  }
  //Console.println("Mean value :");
  //Console.println(cumulAnalogValue/samples);
  return((int)cumulAnalogValue/samples);
   
}

float getDistance()
{
  int valueSensor= recordIR();
  double distanceCM;
  distanceCM = 
  pow(valueSensor, 6)*(2.76075/100000000000000) +
  pow(valueSensor, 5)*(-6.99111/100000000000) +
  pow(valueSensor, 4)*(7.15462/100000000) +
  pow(valueSensor, 3)*(-3.80057/100000) +
  pow(valueSensor, 2)*0.011194502 +
  (valueSensor*(-1.807762943)) +
  146.981348;
  return ((float)distanceCM);
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
	//Console.println("motorForward");
        //Console.println(steptodo);
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
         
        //Console.println("----------Calcul Position--------- ");
        //Sauvegarde du compteur des roues
        encoder_left_saved = encoder_left_saved + step_left;
        encoder_right_saved = encoder_right_saved + step_right;
        /*
        Console.println("encoder_left_saved: ");
        Console.println(encoder_left_saved);
        Console.println("encoder_right_saved: ");
        Console.println(encoder_right_saved);
        */
        // 1 Calcul de la distance parcouru par chaque roue 
        float distance_wheel_left  =  (float) step_left * (float)WHEEL_PERIM / (float)STEPMOTOR;
        float distance_wheel_right = (float) step_right * (float)WHEEL_PERIM /  (float)STEPMOTOR;
        float distance = (distance_wheel_left + distance_wheel_left)/2;
        /*
        Console.println("distance_wheel_left: ");
        Console.println(distance_wheel_left);
        Console.println("distance_wheel_right: ");
        Console.println(distance_wheel_right);
        */ 
        //2 Conversion de l'angle theta en radian
        float thetaRadian = theta * PI / 180;
        /*
        Console.println("thetaRadian: ");
        Console.println(thetaRadian);
        */ 
        //3 Calcul de la position
        float yPrime = distance * cos(thetaRadian);
        float xPrime = distance * sin(thetaRadian);
        /*
        Console.println("yPrime: ");
        Console.println(yPrime);
        Console.println("xPrime: ");
        Console.println(xPrime);
        */
        //Incrementation avec les positions précedentes
        posY = posY + yPrime;
        posX = posX + xPrime;
        /*
        Console.println("posY: ");
        Console.println(posY);
        Console.println("posX: ");
        Console.println(posX);
       */ 
}


//Convertie des degres en step moteur
int calcul_step_turn(int degree_to_turn){

    float x = ((double)degree_to_turn) /((double) 360);
    float distance_for_turn = x * PI * TRACK;
    int step_to_turn = COEFF_CORRECTION + distance_for_turn * STEPMOTOR / WHEEL_PERIM;
   
    //Console.println("steptoturn:");
    //Console.println(step_to_turn);
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
       // Console.println("thetha:");
        //Console.println(theta);
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
        /*Console.println("thetha:");
        Console.println(theta);
        */
}
 
 
float distanceMin=90;
float lastDistance=0;
short int angleDistMin=0;
void test()
{
       
          turnDegreeLeft(50);
          delay(50);

}

void parcoursMainGauche()
{

    switch (etatParcours)
    {
    //recherche du mur le plus proche
      case 1:
      Console.println("cas 1, init");  
        for(int angle=0; angle < 360; angle+=10)
        {
          turnDegreeLeft(10);
          lastDistance= getDistance();
          
          if ( lastDistance < distanceMin)
          {
            distanceMin=lastDistance;
            angleDistMin=angle;
            
          }

        }
        turnDegreeLeft(angleDistMin);
        etatParcours=3;
        break;
        
        
     case 2:
      Console.println("cas 2, balayage");
        //verifier si l'on a bien le point le plus proche a +/-10°
        turnDegreeRight(10);
        for(int angle=0;angle<20;angle++)
        {
          lastDistance= getDistance();
          if ( lastDistance < distanceMin)
          {
            distanceMin=lastDistance;
            angleDistMin=angle;

          }
          turnDegreeLeft(2);

        }
        turnDegreeRight(20-angleDistMin);

        etatParcours = 3;
        break;
        
      case 3:
      Console.println("cas 3, phase d'approche");
        //etat ou l'on avance tout droit jusqu'à être a 7cm du mur
        lastDistance= getDistance();
        if(lastDistance  >= 12)
            motorForward(lastDistance-12);        
        etatParcours=4;
        break;
        
       //tourné a droite
       case 4:
       Console.println("cas 4, tourne a 90 droite");
         turnDegreeRight(90);
         delay(100);
         etatParcours = 5;
         break;
         
        case 5:
        Console.println("cas 5, mur en face entre 7 et 10 cm?");
          //regarder s'il y a un mur en face
          lastDistance= getDistance();
          Console.println(lastDistance);
          if((lastDistance > 7) && (lastDistance < 10))
          {  
            //tournée à droite
            Console.println("oui");
            etatParcours=4;
             break;
          }
          else
          {
            Console.println("non");
             etatParcours=7;
             break;
          }
          
       
        
        //tourné a gauche
       case 6:
       Console.println("cas 6, je tourne de 90 a gauche");
         turnDegreeLeft(90);
         delay(100);
         etatParcours = 5;
         break;
         
         //avancer de 2 cm
         case 7:
         Console.println("cas 7, j'avance");
            motorForward(20);
            delay(100);
            etatParcours = 6;
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

	//Console.println("Simple loop for infrared sensor !");

	pinMode(INFRARED_SENSOR_INPUT, INPUT);
        motorInitialize();
}



void loop()
{
  parcoursMainGauche();
}

