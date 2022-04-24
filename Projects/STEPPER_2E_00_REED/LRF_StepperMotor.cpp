// 
// 
// 

#include "LRF_StepperMotor.h"
#include <StepperMotor.h>


/// <summary>
/// Initializes the motor stepper shield with U2003.
///	e.g. 
///	In 1 = Pin D2
///	In 2 = Pin D3
///	In 3 = Pin D4
///	In 4 = Pin D5
///
///	reed relay 1 contact wired to digital out board 
///	In sensor 1 = Pin D7 (LOW/HIGH)
///	In sensor 2 = Pin D8 (LOW/HIGH)
///
///	The magnet is mounted on one side of horizontal
///	rotating part, one of the reed contacts need to
///	be mounted on platform construction in that way,
///	that it give "magnet" contact to the reed relay
///	when horizontal rotary part is in exact "0°"
///	position, this marks the start direction for
///	"360°" rotation. The second reed relay may mounted
///	on opposite position on platform construction,
///	the sensor is needed only for direction orientation
///	to bring horizontal rotary part in start position
/// </summary>
/// <param name="in1">The In 1 pin.</param>
/// <param name="in2">The In 2 pin.</param>
/// <param name="in3">The In 3 pin.</param>
/// <param name="in4">The In 4 pin.</param>
/// <param name="inStopSensor1">The In stop sensor 1 pin.</param>
/// <param name="inStopSensor2">The In stop sensor 2 pin.</param>
void LRF_StepperMotorClass::init(
	int in1,
	int in2,
	int in3,
	int in4,
	int inStopSensor1,
	int inStopSensor2)
{
	// Set LED_BUILTIN to output
	pinMode(LED_BUILTIN, OUTPUT);
	// Turn off LED
	digitalWrite(LED_BUILTIN, LOW);

	// Motor speed
	motorSpeed = 4;

	// Steps for ~360°
	motorSteps = 4276;// 4630;

	stepsPerOneDegree = motorSteps / 360 + 1;
	// Default destination position
	destinationPosition = 0;

	// Motor step size
	stepSize = 8;

	// Motor direction
	currentDirection = Directions::FORWARD;

	// Create steppe driver instance
	motor = new StepperMotor(2, 3, 4, 5);
	// Apply motor speed
	motor->setStepDuration(motorSpeed);

	// Set stop sensor pins
	stopSensor1 = inStopSensor1;
	stopSensor2 = inStopSensor2;
	pinMode(stopSensor1, INPUT);
	pinMode(stopSensor2, INPUT);

}

/// <summary>
/// Rotate around angle offset to current position.
///	e.g.
///	+n rotate forward CCW
///	-n rotate backward CW
/// </summary>
/// <param name="angleOffset">The angle offset.</param>
void LRF_StepperMotorClass::addAngle(int angleOffset)
{
	// Nothing to do
	if (angleOffset == 0)
		return;

	// Steps offset
	const int offset = angleOffset * stepsPerOneDegree;

	if (offset >= 0)
		currentDirection = Directions::FORWARD;
	else
		currentDirection = Directions::BACKWARD;

	// Add offset to destination
	destinationPosition += offset;

	Serial.print("Position: ");
	Serial.print(currentPosition);
	Serial.print("\t\tOffset: ");
	Serial.print(offset);
	Serial.print("\t\tDestination: ");
	Serial.print(destinationPosition);
	Serial.print("\t\tDirection: ");

	switch (currentDirection)
	{
	case Directions::FORWARD:
		Serial.print("FORWARD");
		break;
	case Directions::BACKWARD:
		Serial.print("BACKWARD");
		break;
	case Directions::HOLD:
		Serial.print("HOLD");
		break;
	case Directions::Unknown:
		Serial.print("Unknown");
		break;
	}

	Serial.print("\t\tSteps/Deg: ");  // NOLINT(clang-diagnostic-invalid-source-encoding)
	Serial.print(stepsPerOneDegree);
	Serial.print("\t\tAngle: ");
	Serial.println(angleOffset);
}

/// <summary>
/// Evaluate one step into destination position.
///	Call this method every "loop" 
/// </summary>
/// <returns>Current moving direction. Returns HOLD when position is reached.</returns>
Directions LRF_StepperMotorClass::step()
{
	// Check if somewhere else
	if (currentPosition != destinationPosition)
	{
		if (currentPosition < destinationPosition)
		{
			// LED ON
			digitalWrite(LED_BUILTIN, HIGH);

			currentDirection = Directions::FORWARD;
			currentPosition += stepSize;
		}
		else
		{
			// LED OFF
			digitalWrite(LED_BUILTIN, LOW);

			currentDirection = Directions::BACKWARD;
			currentPosition -= stepSize;
		}

		// Call step
		switch (currentDirection)
		{
			// Step only when forward direction
		case Directions::FORWARD:
			motor->step(stepSize);
			break;
			// Step only when backward direction
		case Directions::BACKWARD:
			motor->step(-stepSize);
			break;
		case Directions::HOLD:
			// Do nothing and wait for next
			break;
		case Directions::Unknown:
			// Something wont wrong!
			break;
		}

		// Set state
		isHold = false;

		// Re-check if now on hold
		if (currentPosition != destinationPosition)
		{
			// Display motor states on movement
			Serial.print("Position: ");
			Serial.print(currentPosition);
			Serial.print("\t\tDestination: ");
			Serial.print(destinationPosition);
			Serial.print("\t\tDirection: ");
			switch (currentDirection)
			{
			case Directions::FORWARD:
				Serial.print("FORWARD");
				break;
			case Directions::BACKWARD:
				Serial.print("BACKWARD");
				break;
			case Directions::HOLD:
				Serial.print("HOLD");
				break;
			case Directions::Unknown:
				Serial.print("Unknown");
				break;
			}

			Serial.print("\t\tSTS1: ");
			Serial.print(digitalRead(stopSensor1));
			Serial.print(" STS2: ");
			Serial.println(digitalRead(stopSensor2));
		}
	}
	else
	{	// Display motor states always one time on hold
		if (!isHold)
		{
			isHold = true;
			currentDirection = Directions::HOLD;

			Serial.print("Position: ");
			Serial.print(currentPosition);
			Serial.print("\t\tDestination: ");
			Serial.print(destinationPosition);
			Serial.print("\t\tDirection: ");
			switch (currentDirection)
			{
			case Directions::FORWARD:
				Serial.println("FORWARD");
				break;
			case Directions::BACKWARD:
				Serial.println("BACKWARD");
				break;
			case Directions::HOLD:
				Serial.println("HOLD");
				break;
			case Directions::Unknown:
				Serial.println("Unknown");
				break;
			}
		}
	}
	return currentDirection;
}

/// <summary>
/// Resets the current position to "0" == start point.
/// </summary>
void LRF_StepperMotorClass::resetCurrentPosition()
{
	currentPosition = 0;
	destinationPosition = 0;
}

int LRF_StepperMotorClass::getCurrentPosition() const
{
	return currentPosition;
}

void LRF_StepperMotorClass::EvaluateStartPosition()
{
	// Rotate in forward direction 200°
	// Try to detect stop sensor 2
	addAngle(200);

	while (true)
	{
		switch (LRF_StepperMotor.step())
		{
		case Directions::FORWARD:
			//Serial.println("Directions::FORWARD");
			break;
		case Directions::BACKWARD:
			//Serial.println("Directions::BACKWARD");
			break;
		case Directions::HOLD:
			//Serial.println("Directions::HOLD");		
			break;
		case Directions::Unknown:
			//Serial.println("Directions::Unknown");
			break;
		}

		if (digitalRead(stopSensor2) == HIGH)
		{
			// Rotate back from 1 right side to 2 left side
			addAngle(-360);

			while (true)
			{
				LRF_StepperMotor.step();
				if (digitalRead(stopSensor1) == HIGH)
				{
					// Break on correct position
					// and set new current position
					resetCurrentPosition();
					return;
				}
			}
		}
		if (digitalRead(stopSensor1) == HIGH)
		{
			// Rotate back from 2 right to right 2 left
		}
	}
}


LRF_StepperMotorClass LRF_StepperMotor;

