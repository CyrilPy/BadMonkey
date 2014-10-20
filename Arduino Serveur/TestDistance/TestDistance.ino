// Communication test between Arduino and Wifi with the Bridge
//
// After uploading, you can connect to your Arduino Yun with :
// - from the command line : ssh root@yourArduinoName.local 'telnet localhost 6571'
// - from the Arduino serial console
// - from a web browser to http://yourArduinoName.local/arduino/toto/1234

#include <Arduino.h>
#include <Console.h>
#include <YunServer.h>
#include <YunClient.h>

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"


YunServer server;

#define DEFAULT_STEP_NUMBER     100
#define INFRARED_SENSOR_INPUT	A0
#define SAMPLES	                50


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

int * map;
bool run ;
int counter = 0;
int cpt = 0;
int tabIR[SAMPLES];

//Parametres pour l'odometrie
int left_encoder;
int right_encoder;
float theta;
float xPrime;
float yPrime;
float posX ;
float posY ;

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

void setup()
{
	Bridge.begin();
  	Console.begin(); 


  	server.listenOnLocalhost();
  	server.begin();


//	while (!Console)
//  	{
//  		// wait for Console port to connect.
//  	}

  	Console.println("You're connected to the console !");

        Console.println("Test of Stepper Motor !");
      
        motorInitialize();

        pinMode(INFRARED_SENSOR_INPUT, INPUT);

}

long readVcc()
{
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1125300L / result; // Back-calculate AVcc in mV
  return result;
}

void motorForward()
{
  for (int i = 0; i < DEFAULT_STEP_NUMBER; i++)
  {
    motor1->step(1, FORWARD, SINGLE);
    motor2->step(1, FORWARD, SINGLE);
  }
}

void motorBackward()
{
  for (int i = 0; i < DEFAULT_STEP_NUMBER; i++)
  {
    motor1->step(1, BACKWARD, SINGLE);
    motor2->step(1, BACKWARD, SINGLE);
  }
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

void motorTurnRight()
{
  for (int i = 0; i < DEFAULT_STEP_NUMBER; i++)
  {
    motor1->step(1, BACKWARD, SINGLE);
    motor2->step(1, FORWARD, SINGLE);
  }
}

int recordIR()
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
  return((int)cumulAnalogValue/samples);
   
}

void motorTurnLeft()
{
  for (int i = 0; i < DEFAULT_STEP_NUMBER; i++)
  {
    motor1->step(1, FORWARD, SINGLE);
    motor2->step(1, BACKWARD, SINGLE);
  }
}

void sentMap (YunClient* client)
{
  client.println("Status: 200");
  client.println("Content-type: application/json");
  client.println();
  client.println(getMap());
}

void loop()
{
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

	if (command=="fwd")
	{
		Console.println(client.readString());
                Console.println("Move forward...");
                motorForward(50);
	}
	else if (command=="bwd")
	{
		Console.println(client.readString());
                Console.println("Move backward...");
                motorBackward();
	}
	else if (command=="left")
	{
		Console.println(client.readString());
                Console.println("Move left...");
                motorTurnLeft();
	}
	else if (command=="right")
	{
		Console.println(client.readString());
                Console.println("Move right...");
                motorTurnRight();
	}
	else if (command=="record")
	{
		Console.println(client.readString());
                if (cpt <= SAMPLES)
                {
                  Console.println("Recording IR value...");
                  tabIR[cpt] = recordIR();
                  cpt ++;
                }
                else                
                  Console.println("Max number of samples reached");
	}
	else if (command=="getIRtab")
	{              
              Console.println("Displaying tab... cpt="+ cpt);  
              client.println("Status: 200");
              client.println("Content-type: text/html");
              // any other header
              client.println(); //mandatory blank line
	      client.print("<html><body><table>");
	      client.print("<thead><th>Sample</th><th>Value</th></thead>");
	      client.print("<tbody>");
              for (counter = 0; counter <= cpt; counter ++)
              {
                client.print("<tr><td>");
                client.print(counter);
                client.print("</td><td>");
                client.print(tabIR[counter]);
                client.print("</td></tr>");
              }
	      client.print("</tbody>");
	      client.print("</table></body></html>");
              Console.println("Done.");
	}
	else if (command=="reset")
	{              
              Console.println("Reseting tabIR...");
              for(int i = 0; i <= cpt; i++)
                tabIR[i] = 0;
              cpt = 0;
	}
	else if (command=="getdistance")
	{           
              double distance = (double)recordIR();
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
	      client.print(distanceCM );
	}
	else if (command=="getvcc")
	{
	      Console.print((long unsigned)readVcc);
	}
	else if (command=="getmap")
	{ 
              sendMap(&client);
	}
	else
	{
		Console.print("Huuu?? ");
		Console.println(client.readString());
	}

	// send answer to the client:
	//client.print("Got your command : ");
	//client.print(command);

}
