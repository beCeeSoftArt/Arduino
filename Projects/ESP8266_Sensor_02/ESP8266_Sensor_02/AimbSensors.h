// AimbSensors.h

#ifndef AIMBSENSORS_H
#define AIMBSENSORS_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
/*#include "dht.h"
#include "Wire.h"*/
	#include "Adafruit_BMP085.h"

#else
	#include "WProgram.h"
#endif
#include "dht.h"

/// <summary>
/// Class AimbSensorsClass.
/// </summary>
class AimbSensorsClass
{
 protected:
		
	/// <summary>
	/// The DHT sensor
	/// </summary>
	dht dhtSensor;

	/// <summary>
	/// The BMP085 sensor
	/// </summary>
	Adafruit_BMP085 bmp085;

	// DSM501A variables
	uint8_t pinDsm501a;
	unsigned long durationDsm501a;
	unsigned long startTimeDsm501a;
	unsigned long sampletimeDsm501a; // ms
	unsigned long lowPulseOccupancyDsm501a;
	double ratioDsm501a = 0;
	double concentrationDsm501a;
	double lastValueDsm501a;
	
	// Enable flags for sensors
	bool isEnabledDHT22;
	bool isEnabledBMP85;
	bool isEnabledDSM501A;
	bool isEnabledMG811;

public:
		
	/// <summary>
	/// Initializes the DHT22.
	/// </summary>
	void InitDHT22();

	/// <summary>
	/// Initializes this instance.
	/// </summary>
	/// <returns>bool.</returns>
	void InitBMP085();
	
	/// <summary>
	/// Initializes the DSM501A.
	/// </summary>
	void InitDSM501A();

	/// <summary>
	/// Initializes the MG811.
	/// </summary>
	void InitMG811();

	/// <summary>
	/// Enables the dh T22.
	/// </summary>
	/// <param name="isEnabled">The is enabled.</param>
	void EnableDHT22(bool isEnabled);
	
	/// <summary>
	/// Enables the bm P085.
	/// </summary>
	/// <param name="isEnabled">The is enabled.</param>
	void EnableBMP085(bool isEnabled);
	
	/// <summary>
	/// Enables the ds M501 a.
	/// </summary>
	/// <param name="isEnabled">The is enabled.</param>
	void EnableDSM501A(bool isEnabled);
	
	/// <summary>
	/// Enables the m G811.
	/// </summary>
	/// <param name="isEnabled">The is enabled.</param>
	void EnableMG811(bool isEnabled);

	/// <summary>
	/// Creates the check sum for NMEA sentence.
	/// </summary>
	/// <param name="nmeaSentence">The NMEA sentence.</param>
	/// <returns>int.</returns>
	int CreateCheckSum(String nmeaSentence);
	
	/// <summary>
	/// Creates checks sum an convert to hexadecimal value.
	/// </summary>
	/// <param name="nmeaSentence">The NMEA sentence.</param>
	/// <returns>String.</returns>
	String CreateCheckSumHex(String nmeaSentence);

	/// <summary>
	/// Gets the DHT22 sensor data messages.
	/// </summary>
	/// <param name="sensorId">The sensor identifier.</param>
	/// <param name="pin">The pin.</param>
	/// <returns>String.</returns>
	String GetDht22Data(int sensorId, uint8_t pin);

	/// <summary>
	/// Gets the BMP085 data.
	/// </summary>
	/// <param name="sensorId">The sensor identifier.</param>
	/// <returns>String.</returns>
	String GetBmp085Data(int sensorId);

	/// <summary>
	/// Gets the dsm501a data.
	/// </summary>
	/// <returns>String.</returns>
	void UpdateDsm501aData();

	/// <summary>
	/// Gets the dsm501a data.
	/// </summary>
	/// <param name="sensorId">The sensor identifier.</param>
	/// <returns>String.</returns>
	String GetDsm501aData(int sensorId);

	/// <summary>
	/// Gets the MG811 CO2 value.
	/// </summary>
	/// <param name="sensorId">The sensor identifier.</param>
	/// <returns>String.</returns>
	String GetMg811(int sensorId);
};

extern AimbSensorsClass AimbSensors;

#endif

