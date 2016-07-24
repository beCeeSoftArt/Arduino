/*
	Copyright André Spitzner, 2003 - 2012

	beCee Electro Engine Implementation
*/

// Includes
extern "C" 
{
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}

#include <Arduino.h>
#include "TB6612FNG.h"

// Initialize Class Variables /////////////////////////////////////////////////

int TB6612FNG::STBY = 10;

// Constructors ////////////////////////////////////////////////////////////////

/// <summary>
/// Initializes a new instance of the <see cref="TB6612FNG" /> class.
/// </summary>
/// <param name="iPWMa">The PWA engine a (3).</param>
/// <param name="iIN1a">The In1 engine a (9).</param>
/// <param name="iIN2a">The In2 engine a (8).</param>
/// <param name="iPWMb">The PWM engine b (5).</param>
/// <param name="iIN1b">The In1 engine b (11).</param>
/// <param name="iIN2b">The In2 engine b (12).</param>
TB6612FNG::TB6612FNG(int iPWMa, int iIN1a, int iIN2a, int iPWMb, int iIN1b, int iIN2b)
{
	oEngine1.iPWM = 3;
	oEngine1.iIN1 = 9;
	oEngine1.iIN2 = 8;

	oEngine2.iPWM = 5;
	oEngine2.iIN1 = 11;
	oEngine2.iIN2 = 12;

	pinMode(STBY, OUTPUT);
	pinMode(oEngine1.iPWM, OUTPUT); 
	pinMode(oEngine1.iIN1, OUTPUT);
	pinMode(oEngine1.iIN2, OUTPUT);
	pinMode(oEngine2.iPWM, OUTPUT); 
	pinMode(oEngine2.iIN1, OUTPUT); 
	pinMode(oEngine2.iIN2, OUTPUT);
}

/// <summary>
/// Engines the power.
/// </summary>
/// <param name="iEngineNumber">The engine number.</param>
/// <param name="iDirection">The direction.</param>
/// <param name="iSpeed">The speed.</param>
void TB6612FNG::EnginePower(int iEngineNumber, int iDirection, int iSpeed)
{ 
	//Move specific motor at speed and direction 
	//speed: 0 is off, and 255 is full speed 
	//direction: 0 clockwise, 1 counter-clockwise
	digitalWrite(STBY, HIGH); //disable standby
	boolean inPin1 = LOW; 
	boolean inPin2 = HIGH;
	if(iDirection == 1)
	{
		inPin1 = HIGH; 
		inPin2 = LOW; 
	}

	switch (iEngineNumber)
	{
		case 0:
			digitalWrite(oEngine1.iIN1, inPin1); 
			digitalWrite(oEngine1.iIN2, inPin2); 
			analogWrite(oEngine1.iPWM, iSpeed); 
			break;
		case 1:
			digitalWrite(oEngine2.iIN1, inPin1); 
			digitalWrite(oEngine2.iIN2, inPin2); 
			analogWrite(oEngine2.iPWM, iSpeed); 
			break;
	}
}

/// <summary>
/// Standby.
/// </summary>
void TB6612FNG::Standby()
{ 
	// Enable standby  
	digitalWrite(STBY, LOW); 
}

// Instantiated Objects //////////////////////////////////////////////////////

/// <summary>
/// The protocol already usage instance
/// </summary>
TB6612FNG oTB6612FNG = TB6612FNG(3, 9, 8, 5, 11, 12);

