// LRF_StepperMotor.h

#ifndef LRF_STEPPERMOTOR_H
#define LRF_STEPPERMOTOR_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include "WProgram.h"
#endif

#include <StepperMotor.h>

typedef enum
    {
		Unknown = -1,
		HOLD  = 0,
		FORWARD    = 1,
		BACKWARD = 2
    } Directions;

class LRF_StepperMotorClass
{
protected:

	// Stepper motor instance
	StepperMotor* motor = nullptr;

	// Motor speed
	int motorSpeed = 8;

	// Steps for ~360°
	int motorSteps = 3960;//4276;//4096;

	// Steps per "one degree"
	int stepsPerOneDegree = 0;

	// Step values
	int  currentPosition = 0;

	// Destination position
	int  destinationPosition = 0;

	// Step size
	int stepSize = 8;

	// Motor direction
	Directions currentDirection = Directions::Unknown;

	// Pin stop sensor 1
	int stopSensor1 = 7;

	// Pin stop sensor 2
	int stopSensor2 = 6;

private:
	bool isHold = true;

public:

	void init(int in1, int in2, int in3, int in4, int stopSensor1, int stopSensor2);

	void addAngle(int angleOffset);

	Directions step();

	void resetCurrentPosition();

	int getCurrentPosition() const;

	void EvaluateStartPosition();

};

extern LRF_StepperMotorClass LRF_StepperMotor;

#endif

