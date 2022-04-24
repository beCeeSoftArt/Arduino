// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
	Name:       STEPPER_2E_00_REED.ino
	Created:	02.01.2022 21:11:58
	Author:     AURORA-R10\bcare
*/

#include <Wire.h>
#include "LRF_StepperMotor.h"
#include <StepperMotor.h>

// IN1, IN2, IN3, IN4
StepperMotor motor(2, 3, 4, 5);
int motorSpeed = 8;

int stopSensor0 = 7;
int stopSensor1 = 6;

// ~360°
int motorSteps = 4630;

// Step values
int  currentPosition = 0;
int  destinationPosition = 0;
int stepSize = 9;
bool direction = false;

void setup()
{
	Serial.begin(115200);

	//// Set LED_BUILTIN to output
	//pinMode(LED_BUILTIN, OUTPUT);
	//// Turn off LED
	//digitalWrite(LED_BUILTIN, LOW);

	//pinMode(stopSensor0, INPUT);
	//pinMode(stopSensor1, INPUT);
	//

	//// Set default motor speed
	//motor.setStepDuration(motorSpeed);

	//destinationPosition = motorSteps;
	//direction = true;

	// Initialize motor driver
	LRF_StepperMotor.init(2, 3, 4, 5, 6, 7);

	LRF_StepperMotor.EvaluateStartPosition();

	//LRF_StepperMotor.addAngle(-360);
}

void loop()
{
	return;
	switch (LRF_StepperMotor.step())
	{
	case Directions::FORWARD:
		//Serial.println("Directions::FORWARD");
		break;
	case Directions::BACKWARD:
		//Serial.println("Directions::BACKWARD");
		break;
	case Directions::HOLD:
		Serial.println("Directions::HOLD");

		if(direction)
		{
			direction = false;
			LRF_StepperMotor.addAngle(-360);
		}
		else
		{
			direction = true;
			LRF_StepperMotor.addAngle(360);
		}
		break;
	case Directions::Unknown:
		//Serial.println("Directions::Unknown");
		break;
	}
			
	//delay(10);
	//// Forward
	//if (currentPosition > destinationPosition && direction == true)
	//{
	//	// LED ON
	//	digitalWrite(LED_BUILTIN, HIGH);

	//	direction = false;
	//	currentPosition = 0;
	//}

	//// Reverse
	//if (currentPosition > destinationPosition && direction == false)
	//{
	//	// LED OFF
	//	digitalWrite(LED_BUILTIN, LOW);

	//	direction = true;
	//	currentPosition = 0;
	//}

	//if (direction)
	//	motor.step(stepSize);
	//else
	//	motor.step(-stepSize);

	//Serial.print("Position: ");
	//Serial.print(currentPosition);
	//Serial.print("\t\tDestination: ");
	//Serial.print(destinationPosition);
	//Serial.print("\t\tDirection: ");
	//Serial.print(direction == true ? "Forward" : "Backward");
	//Serial.print("\t\tST0: ");
	//Serial.print(digitalRead(stopSensor0));
	//Serial.print("\t\tST1: ");
	//Serial.println(digitalRead(stopSensor1));

	//currentPosition += stepSize;
	//delay(10);
}
