// 
// 
// 

#include "AimbSensors.h"

/// <summary>
/// Initializes the DHT22.
/// </summary>
void AimbSensorsClass::InitDHT22()
{
	/*if (!isEnabledDHT22)
		return;*/

}

/// <summary>
/// Initializes the BMP085.
/// </summary>
void AimbSensorsClass::InitBMP085()
{
	if (!isEnabledBMP85)
		return;
	
	// Check if theirs a valid BMP085 sensor connected
	if (!bmp085.begin())
		while (true)
		{
			Serial.println("Could not find a valid BMP085 sensor, check wiring!");
			delay(1000);
		}
}

/// <summary>
/// Initializes the DSM501A.
/// </summary>
void AimbSensorsClass::InitDSM501A()
{
	if (!isEnabledDSM501A)
		return;

	pinDsm501a = 16;
	durationDsm501a = 0;
	sampletimeDsm501a = 5000; // ms
	lowPulseOccupancyDsm501a = 0;
	ratioDsm501a = 0;
	concentrationDsm501a = 0;
	lastValueDsm501a = 0;

	pinMode(pinDsm501a, INPUT);
	// Get the current time;
	startTimeDsm501a = millis();
}

/// <summary>
/// Initializes the MG811.
/// </summary>
void AimbSensorsClass::InitMG811()
{
	/*if (!isEnabledMG811)
		return;*/

}

/// <summary>
/// Enables the dh T22.
/// </summary>
/// <param name="isEnabled">The is enabled.</param>
void AimbSensorsClass::EnableDHT22(bool isEnabled)
{
	isEnabledDHT22 = isEnabled;
}

/// <summary>
/// Enables the bm P085.
/// </summary>
/// <param name="isEnabled">The is enabled.</param>
void AimbSensorsClass::EnableBMP085(bool isEnabled)
{
	isEnabledBMP85 = isEnabled;
}

/// <summary>
/// Enables the ds M501 a.
/// </summary>
/// <param name="isEnabled">The is enabled.</param>
void AimbSensorsClass::EnableDSM501A(bool isEnabled)
{
	isEnabledDSM501A = isEnabled;
}

/// <summary>
/// Enables the m G811.
/// </summary>
/// <param name="isEnabled">The is enabled.</param>
void AimbSensorsClass::EnableMG811(bool isEnabled)
{
	isEnabledMG811 = isEnabled;
}

/// <summary>
/// Creates the check sum for NMEA sentence.
/// </summary>
/// <param name="nmeaSentence">The NMEA sentence.</param>
/// <returns>int.</returns>
int AimbSensorsClass::CreateCheckSum(String nmeaSentence)
{
	int i;
	int iXOR;
	int c;
	// Calculate checksum ignoring any $'s in the string
	for (iXOR = 0, i = 0; i < strlen(nmeaSentence.c_str()); i++)
	{
		c = static_cast<unsigned char>(nmeaSentence[i]);
		if (c == '*') break;
		if (c != '$') iXOR ^= c;
	}
	return iXOR;
}

/// <summary>
/// Creates checks sum an convert to hexadecimal value.
/// </summary>
/// <param name="nmeaSentence">The nmea sentence.</param>
/// <returns>String.</returns>
String AimbSensorsClass::CreateCheckSumHex(String nmeaSentence)
{
	String result = "";
	char tmp1[10];
	sprintf(tmp1, "%X", CreateCheckSum(nmeaSentence));
	result += tmp1;
	return result;
}

/// <summary>
/// Gets the DHT22 sensor data messages.
/// HUMIDITY and TEMP, when sensor fails returns ERROR message
/// </summary>
/// <param name="sensorId">The sensor identifier.</param>
/// <param name="pin">The pin.</param>
/// <returns>String.</returns>
String AimbSensorsClass::GetDht22Data(int sensorId, uint8_t pin)
{
	if (!isEnabledDHT22)
		return "";

	String result = "";
	String resultTemp = "";

	int chk = dhtSensor.read22(pin);//read the value returned from sensor
	switch (chk)
	{
	case DHTLIB_OK:

		resultTemp = "$BCHUMIDITY,";
		resultTemp += sensorId;
		resultTemp += ",";
		resultTemp += dhtSensor.humidity;
		resultTemp += ",%RH*";
		resultTemp += CreateCheckSumHex(resultTemp);
		resultTemp += "\n";
		result += resultTemp;
		
		resultTemp ="$BCTEMP,";
		resultTemp += sensorId;
		resultTemp +=",";
		resultTemp += dhtSensor.temperature;
		resultTemp += ",C";
		resultTemp += "*";
		resultTemp += CreateCheckSumHex(resultTemp);
		result += resultTemp;

		break;
	case DHTLIB_ERROR_CHECKSUM:
		
		result = "$BCERROR,";
		result += pin;
		result += ",";
		result += DHTLIB_ERROR_CHECKSUM;
		result += ",Checksum error";
		result += "*";
		result += CreateCheckSumHex(result);

		break;
	case DHTLIB_ERROR_TIMEOUT:

		result = "$BCERROR,";
		result += pin;
		result += ",";
		result += DHTLIB_ERROR_TIMEOUT;
		result += ",Time out humidity/temperature sensor error";
		result += "*";
		result += CreateCheckSumHex(result);

		break;
	case DHTLIB_INVALID_VALUE:

		result = "$BCERROR,";
		result += pin;
		result += ",";
		result += DHTLIB_INVALID_VALUE;
		result += ",Invalid value";
		result += "*";
		result += CreateCheckSumHex(result);

		break;
	default:
	
		result = "$BCERROR,";
		result += pin;
		result += ",";
		result += 0;
		result += ",Unknown humidity/temperature sensor error";
		result += "*";
		result += CreateCheckSumHex(result);

		break;
	}

	result += "\n";
	return result;
}

/// <summary>
/// Gets the BMP085 data.
/// </summary>
/// <param name="sensorId">The sensor identifier.</param>
/// <returns>String.</returns>
String AimbSensorsClass::GetBmp085Data(int sensorId)
{
	if (!isEnabledBMP85)
		return "";

	String result = "";
	String resultTemp = "";

	resultTemp = "$BCTEMP,";
	resultTemp += sensorId;
	resultTemp += ",";
	resultTemp += bmp085.readTemperature();
	resultTemp += ",C";
	resultTemp += "*";
	resultTemp += CreateCheckSumHex(resultTemp);
	resultTemp += "\n";
	result += resultTemp;

	resultTemp = "$BCPRESSURE,";
	resultTemp += sensorId;
	resultTemp += ",";
	resultTemp += bmp085.readPressure() / 100;
	resultTemp += ",";
	resultTemp += "mBar";
	resultTemp += "*";
	resultTemp += CreateCheckSumHex(resultTemp);
	resultTemp += "\n";
	result += resultTemp;

	resultTemp = "$BCALTITUDE,";
	resultTemp += sensorId;
	resultTemp += ",";
	resultTemp += bmp085.readAltitude(102900);
	resultTemp += ",";
	resultTemp += "m";
	resultTemp += "*";
	resultTemp += CreateCheckSumHex(resultTemp);
	resultTemp += "\n";
	result += resultTemp;

	return result;
}

/// <summary>
/// Updates the dsm501a data.
/// </summary>
void AimbSensorsClass::UpdateDsm501aData()
{
	if (!isEnabledDSM501A)
		return;

	durationDsm501a = pulseIn(pinDsm501a, LOW);
	lowPulseOccupancyDsm501a = lowPulseOccupancyDsm501a + durationDsm501a;

	if ((millis() - startTimeDsm501a) >= sampletimeDsm501a)
	{
		ratioDsm501a = lowPulseOccupancyDsm501a / (sampletimeDsm501a * 10.0);  // Integer percentage 0=&gt;100
		concentrationDsm501a = 1.1 * pow(ratioDsm501a, 3) - 3.8 * pow(ratioDsm501a, 2) + 520 * ratioDsm501a + 0.62; // using spec sheet curve

		lastValueDsm501a = lastValueDsm501a * 0.9 + concentrationDsm501a * 0.1;

		// Reset
		lowPulseOccupancyDsm501a = 0;
		startTimeDsm501a = millis();
	}
}

/// <summary>
/// Gets the dsm501a data.
/// </summary>
/// <param name="sensorId">The sensor identifier.</param>
/// <returns>String.</returns>
String AimbSensorsClass::GetDsm501aData(int sensorId)
{
	if (!isEnabledDSM501A)
		return "";

	char tmp1[10];
	dtostrf(lastValueDsm501a, 3, 2, tmp1);
	String result = "$BCDUST,";
	result += sensorId;
	result += ",";
	result += tmp1;
	result += ",0.01cf*";
	result += CreateCheckSumHex(result);
	result += "\n";
	return result;
}

/// <summary>
/// Gets the MG811.
/// </summary>
/// <param name="sensorId">The sensor identifier.</param>
/// <returns>String.</returns>
String AimbSensorsClass::GetMg811(int sensorId)
{
	if (!isEnabledMG811)
		return "";

	char tmp1[10];

	unsigned int co2value = analogRead(A0);
	co2value = map(co2value, 0, 1023, 350, 10000);

	ultoa(co2value, &tmp1[0], DEC);
	String result = "$BCCO2,";
	result += sensorId;
	result += ",";
	result += tmp1;
	result += ",ppm*";
	result += CreateCheckSumHex(result);
	result += "\n";
	return result;
}

AimbSensorsClass AimbSensors;

