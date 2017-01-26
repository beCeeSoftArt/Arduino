#include <OneWire.h>
#include "bcplanet_dsb.h"

OneWire  oneWire(ONEWIRE_PIN);
bcplanet_dsb DSB;

void setup()
{
#if defined (__AVR_ATmega32U4__) // no ASSR register for Arduino Leonardo
	while (!Serial)
	{
		; // wait for serial port to connect. Needed for Leonardo only
	}
#endif
	Serial.begin(115200);
	// Set OneWire instance
	DSB.SetOneWireReference(&oneWire);
}

void loop()
{
	// Reset stop watch
	DSB.StartElapsedTime();

	// Analog 3 Build-In CO2
	DSB.WriteGas(20, 3, 307, "CO2");
	// Analog 3 Build-In NH3
	DSB.WriteGas(30, 3, 0, "NH3");

	// Analog 0 CO2
	DSB.WriteGas(21, 0, 340.2, "CO2");
	// Analog 0 NH3
	DSB.WriteGas(31, 0, 0, "NH3");

	// Analog 1 CO2
	DSB.WriteGas(22, 1, 369.2, "CO2");
	// Analog 1 NH3
	DSB.WriteGas(32, 1, 0, "NH3");
	//delay(500);

	// Digital 9 Build-In
	DSB.WritePir(40);
	// Digital 7
	DSB.WriteHumidityTemperatureDHT(8, 22, 0, -14);
	// Digital 8
	DSB.WriteHumidityTemperatureDHT(7, 11, 4, -55);
	// Analog 0-5
	DSB.WriteAnalogPlain(0);
	// Analog 0-5
	DSB.WriteAnalog(0);
	// Analog 2
	DSB.WriteHumidityHIH4030(2, 10);

	// Device data
	DSB.WriteFreeRam(0);
	// Device data
	DSB.WriteProgramSize(0);

	// Execution duration
	DSB.WriteElapsedTime(0);
	// Reset stop watch
	DSB.StartElapsedTime();

	// Digital 2 One Wire Sensor 1
	DSB.WriteTemperatureDS18B20();
	// Digital 2 One Wire Sensor 2
	DSB.WriteTemperatureDS18B20();
	
	// Execution duration
	DSB.WriteElapsedTime(1);

	//delay(500);
}


