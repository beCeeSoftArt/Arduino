/*
	Copyright André Spitzner, 2003 - 2012
  
	beCee AIMB Protocol Implementation
*/

#ifndef TB6612FNG_h
#define TB6612FNG_h

#include <inttypes.h>


/// <summary>
/// Engine
/// </summary>
struct Engine
{
	/// <summary>
	/// The PWM
	/// </summary>
	int iPWM; 
	/// <summary>
	/// The Speed Control 
	/// </summary>
	int iIN1; 
	/// <summary>
	/// The Direction
	/// </summary>
	int iIN2; 
};

/// <summary>
/// TB6612FNG motor driver class
/// </summary>
class TB6612FNG
{
	private:

		static int STBY; //standby

		Engine oEngine1;
		Engine oEngine2;
	public:
		// Base Constructor
		TB6612FNG(int iPWMa, int iIN1a, int iIN2a, int iPWMb, int iIN1b, int iIN2b);

		// Set Engines power
		void EnginePower(int iEngineNumber, int iDirection, int iSpeed);
		// Stop
		void Standby();
};

/*
	Already Usage Instance
*/
extern TB6612FNG oTB6612FNG;


#endif

