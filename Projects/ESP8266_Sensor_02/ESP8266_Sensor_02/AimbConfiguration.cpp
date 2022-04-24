// 
// 
// 

#include "AimbConfiguration.h"

/// <summary>
/// Initializes a new instance of the <see cref="AimbConfiguration" /> class.
/// </summary>

AimbConfiguration::AimbConfiguration()
= default;

void AimbConfiguration::WriteToEeprom(String buffer, int startIndex)
{
	EEPROM.begin(512);
	delay(10);
	
	for (auto index = 0; index < 32; ++index) 
		EEPROM.write(startIndex + index, buffer[index]);

	EEPROM.commit();
}

String AimbConfiguration::ReadFromEeprom(int startIndex, int endIndex)
{
	EEPROM.begin(512);
	delay(10);
	
	String buffer;
	
	for (auto index = startIndex; index < endIndex; ++index)
		if (isAlphaNumeric(EEPROM.read(index)))
			buffer += static_cast<char>(EEPROM.read(index));
	return buffer;
}
