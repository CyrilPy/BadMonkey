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
#define SAMPLES	                11


Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor * motor1 = AFMS.getStepper(200, 2);
Adafruit_StepperMotor * motor2 = AFMS.getStepper(200, 1);

bool run ;
int counter = 0;
int cpt = 0;
int tabIR[SAMPLES];

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

/*String ()
{
  for (int i = 0; i < DEFAULT_STEP_NUMBER; i++)
  {
    motor1->step(1, FORWARD, SINGLE);
    motor2->step(1, BACKWARD, SINGLE);
  }
}*/

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
                motorForward();
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
              //Polynome : x*3,09806E-14	-7,88745E-11	8,10272E-08	-4,30409E-05	0,012581264	-1,991097159	156,2339364              
              double distanceCM;
              distanceCM = 
              pow(distance, 6)*(3.09806/100000000000000) +
              pow(distance, 5)*(-7.88745/100000000000) +
              pow(distance, 4)*(8.10272/100000000) +
              pow(distance, 3)*(-4.30409/100000) +
              pow(distance, 2)*0.012581264 +
              (distance*(-1.991097159)) +
              156.2339364;
	      client.print(distanceCM );
	}
	else if (command=="getmap")
	{           
              double distance = (double)recordIR();
              //Polynome : x*3,09806E-14	-7,88745E-11	8,10272E-08	-4,30409E-05	0,012581264	-1,991097159	156,2339364              
              double distanceCM;
              distanceCM = 
              pow(distance, 6)*(3.09806/100000000000000) +
              pow(distance, 5)*(-7.88745/100000000000) +
              pow(distance, 4)*(8.10272/100000000) +
              pow(distance, 3)*(-4.30409/100000) +
              pow(distance, 2)*0.012581264 +
              (distance*(-1.991097159)) +
              156.2339364;
	      client.print(distanceCM );
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
