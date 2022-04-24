// AimbConfiguration.h

#ifndef _AIMBCONFIGURATION_h
#define _AIMBCONFIGURATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <EEPROM.h>

/// <summary>
/// Class AimbConfiguration.
/// </summary>
class AimbConfiguration
{
private:
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="AimbConfiguration" /> class.
	/// </summary>
	AimbConfiguration();

	/// <summary>
	/// Writes to eeprom.
	/// </summary>
	/// <param name="buffer">The buffer.</param>
	/// <param name="startIndex">The start index.</param>
	void WriteToEeprom(String buffer, int startIndex);

	/// <summary>
	/// Reads from EEPROM.
	/// </summary>
	/// <param name="startIndex">The start index.</param>
	/// <param name="endIndex">The end index.</param>
	/// <returns>String.</returns>
	String ReadFromEeprom(int startIndex, int endIndex);
};

/*
	Already Usage Instance
*/
extern AimbConfiguration Configuration;

#endif

