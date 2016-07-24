// 
// 
// 

#include "bcplanet_dsb.h"

/// <summary>
/// Initializes a new instance of the <see cref="bcplanet_dsb"/> class.
/// </summary>
bcplanet_dsb::bcplanet_dsb()
{
	MQ135_DEFAULTPPM = 404; //default ppm of CO2 for calibration
	MQ135_DEFAULTRO = 41763; //default Ro for MQ135_DEFAULTPPM ppm of CO2
	MQ135_SCALINGFACTOR = 116.6020682; //CO2 gas value
	MQ135_EXPONENT = -2.769034857; //CO2 gas value
	MQ135_MAXRSRO = 2.428; //for CO2
	MQ135_MINRSRO = 0.358; //for CO2

	ADC_REFRES = 1024; //reference resolution used for conversions

	pinMode(PIR_PIN, INPUT);
	pinMode(13, OUTPUT);
	digitalWrite(PIR_PIN, LOW);
	//ds = OneWire(ONEWIRE_PIN);
}

/// <summary>
/// Sets the one wire.
/// </summary>
/// <param name="oneWireRef">The one wire reference.</param>
void bcplanet_dsb::SetOneWireReference(OneWire* oneWireRef)
{
	oneWire = oneWireRef;
}

/// <summary>
/// Writes the temperature DS18B20.
/// </summary>
/// <param name="sensorId">The sensor identifier.</param>
void bcplanet_dsb::WriteTemperatureDS18B20(int sensorId)
{
	byte i;
	byte present = 0;
	byte type_s;
	byte data[12];
	byte addr[8];
	float celsius, fahrenheit;

	if (!oneWire->search(addr))
	{
		// Serial.println("No more addresses.");
		//Serial.println();
		oneWire->reset_search();
		delay(250);
		return;
	}

	// Serial.print("ROM =");
	// for (i = 0; i < 8; i++)
	// {
		// if (i < 7)
			// 	Serial.write(' ');
			// 	Serial.print(addr[i], HEX);
			// }

	if (OneWire::crc8(addr, 7) != addr[7])
	{
		// Serial.print("\tCRC is not valid!");
		return;
	}
	//Serial.println();

	// the first ROM byte indicates which chip
	switch (addr[0])
	{
	case 0x10:
		// Serial.print("\tChip = DS18S20 or old DS1820");  // or old DS1820
		type_s = 1;
		break;
	case 0x28:
		// Serial.print("\tChip = DS18B20");
		type_s = 0;
		break;
	case 0x22:
		// Serial.print("\tChip = DS1822");
		type_s = 0;
		break;
	default:
		WriteError(sensorId, 0, "Device is not a DS18x20 family device.");
		//Serial.print("\tDevice is not a DS18x20 family device.");
		return;
	}

	oneWire->reset();
	oneWire->select(addr);
	oneWire->write(0x44, 1);        // start conversion, with parasite power on at the end

	delay(1000);     // maybe 750ms is enough, maybe not
					 // we might do a ds.depower() here, but the reset will take care of it.

	present = oneWire->reset();
	oneWire->select(addr);
	oneWire->write(0xBE);         // Read Scratchpad

								  // Serial.print("\tData = ");
								  // Serial.print(present, HEX);
								  // Serial.print(" ");
	for (i = 0; i < 9; i++) {           // we need 9 bytes
		data[i] = oneWire->read();
		// Serial.print(data[i], HEX);
		// Serial.print(" ");
	}
	// Serial.print("\tCRC=");
	// Serial.print(OneWire::crc8(data, 8), HEX);
	// Serial.print("\t");

	// Convert the data to actual temperature
	// because the result is a 16 bit signed integer, it should
	// be stored to an "int16_t" type, which is always 16 bits
	// even when compiled on a 32 bit processor.
	int16_t raw = (data[1] << 8) | data[0];
	if (type_s)
	{
		raw = raw << 3; // 9 bit resolution default
		if (data[7] == 0x10)
		{
			// "count remain" gives full 12 bit resolution
			raw = (raw & 0xFFF0) + 12 - data[6];
			// 	Serial.print("\tResolution = 12 bit");
		}
		// else
			// 	Serial.print("\tResolution = 9 bit");
	}
	else
	{
		byte cfg = (data[4] & 0x60);
		// at lower res, the low bits are undefined, so let's zero them
		if (cfg == 0x00)
		{
			raw = raw & ~7;  // 9 bit resolution, 93.75 ms
							 // 	Serial.print("\tResolution = 9 bit");
		}
		else if (cfg == 0x20)
		{
			raw = raw & ~3; // 10 bit res, 187.5 ms
							// 	Serial.print("\tResolution = 10 bit");
		}
		else if (cfg == 0x40)
		{
			raw = raw & ~1; // 11 bit res, 375 ms
							// 	Serial.print("\tResolution = 11 bit");
		}
		// else
		// 	Serial.print("\tResolution = 12 bit");
		// default is 12 bit resolution, 750 ms conversion time
	}
	celsius = static_cast<float>(raw) / 16.0;
	fahrenheit = celsius * 1.8 + 32.0;
	WriteTemperatureSentence(sensorId, celsius);
	/*Serial.print("\tTemperature = ");
	Serial.print(celsius);
	Serial.print(" Celsius, ");
	Serial.print(fahrenheit);
	Serial.println(" Fahrenheit");*/
}

/// <summary>
/// Prints the sentence.
/// </summary>
void bcplanet_dsb::PrintSentence()
{
	sprintf(m_cLinePrint, "%X", CreateCheckSum(m_cLineTemp));

	m_sSentence = m_cLineTemp;
	m_sSentence += m_cLinePrint;
	Serial.println(m_sSentence);
}

/// <summary>
/// Creates the check sum.
/// </summary>
/// <param name="pNMEA">The nmea string.</param>
/// <returns></returns>
int bcplanet_dsb::CreateCheckSum(char *pNMEA)
{
	int i;
	int iXOR;
	int c;
	// Calculate checksum ignoring any $'s in the string
	for (iXOR = 0, i = 0; i < strlen(pNMEA); i++)
	{
		c = static_cast<unsigned char>(pNMEA[i]);
		if (c == '*') break;
		if (c != '$') iXOR ^= c;
	}
	return iXOR;
}

/// <summary>
/// Frees the ram.
/// </summary>
/// <returns></returns>
int bcplanet_dsb::FreeRam()
{
	extern int __heap_start;
	extern int *__brkval;
	int v;
	return reinterpret_cast<int>(&v) - (__brkval == 0 ? reinterpret_cast<int>(&__heap_start) : reinterpret_cast<int>(__brkval));
}

/// <summary>
/// Programs the size.
/// </summary>
/// <returns></returns>
unsigned long bcplanet_dsb::ProgramSize()
{
	extern int _etext;
	extern int _edata;

	return (reinterpret_cast<unsigned long>(&_etext) + (reinterpret_cast<unsigned long>(&_edata) - 256L));
}


/// <summary>
/// Converts to voltage.
/// </summary>
/// <param name="fValue">The f value.</param>
/// <returns></returns>
float bcplanet_dsb::ConvertToVoltage(float fValue)
{
	float fRet = fValue * (5.0 / 1023.0);
	return (fRet);
}

// Service

/// <summary>
/// Writes the free ram.
/// </summary>
/// <param name="sensorId">The sensor identifier.</param>
void bcplanet_dsb::WriteFreeRam(int sensorId)
{
	sprintf(m_cLineTemp, "$BCFREERAM,%i,%i,Byte*",
		sensorId,
		FreeRam());
	PrintSentence();
}

/// <summary>
/// Writes the current program size.
/// </summary>
/// <param name="sensorId">The sensor identifier.</param>
void bcplanet_dsb::WriteProgramSize(int sensorId)
{
	sprintf(m_cLineTemp, "$BCPROGSIZE,%i,%i,Byte*",
		sensorId,
		ProgramSize());
	PrintSentence();
}

/// <summary>
/// Writes the error.
/// </summary>
/// <param name="sensorId">The sensor identifier.</param>
/// <param name="errorId">The error identifier.</param>
/// <param name="pMessage">The p message.</param>
void bcplanet_dsb::WriteError(int sensorId, int errorId, char* pMessage)
{
	sprintf(m_cLineTemp, "$BCERROR,%i,%i,%s*",
		sensorId,
		errorId,
		pMessage);
	PrintSentence();
}

/// <summary>
/// Writes the analog.
/// </summary>
/// <param name="sensorId">The sensor identifier.</param>
void bcplanet_dsb::WriteAnalog(int sensorId)
{
	char tmp1[20];
	char tmp2[20];
	char tmp3[20];
	char tmp4[20];
	char tmp5[20];
	char tmp6[20];
	dtostrf(ConvertToVoltage(analogRead(A0)), 3, 5, tmp1);
	dtostrf(ConvertToVoltage(analogRead(A1)), 3, 5, tmp2);
	dtostrf(ConvertToVoltage(analogRead(A2)), 3, 5, tmp3);
	dtostrf(ConvertToVoltage(analogRead(A3)), 3, 5, tmp4);
	dtostrf(ConvertToVoltage(analogRead(A4)), 3, 5, tmp5);
	dtostrf(ConvertToVoltage(analogRead(A5)), 3, 5, tmp6);
	sprintf(m_cLineTemp, "$BCANALOG,%i,%s,V,%s,V,%s,V,%s,V,%s,V,%s,V*",
		sensorId,
		tmp1,
		tmp2,
		tmp3,
		tmp4,
		tmp5,
		tmp6);
	PrintSentence();
}

/// <summary>
/// Writes the analog plain.
/// </summary>
/// <param name="sensorId">The sensor identifier.</param>
void bcplanet_dsb::WriteAnalogPlain(int sensorId)
{
	sprintf(m_cLineTemp, "$BCANALOGPLAIN,%i,%i,%i,%i,%i,%i,%i*",
		sensorId,
		analogRead(A0),
		analogRead(A1),
		analogRead(A2),
		analogRead(A3),
		analogRead(A4),
		analogRead(A5));
	PrintSentence();
}

/// <summary>
/// Writes the humidity digital.
/// </summary>
/// <param name="iDigitalPort">The i digital port.</param>
/// <param name="sensorValue">The sensor value.</param>
void bcplanet_dsb::WriteHumiditySentence(int sensorId, float sensorValue)
{
	char tmp1[20];
	dtostrf(sensorValue, 3, 5, tmp1);
	sprintf(m_cLineTemp, "$BCHUMIDITY,%i,%s,RH*",
		sensorId,
		tmp1);
	PrintSentence();
}

/// <summary>
/// Writes the temperature digital.
/// </summary>
/// <param name="iDigitalPort">The i digital port.</param>
/// <param name="sensorValue">The sensor value.</param>
void bcplanet_dsb::WriteTemperatureSentence(int sensorId, float sensorValue)
{
	char tmp1[20];
	dtostrf(sensorValue, 3, 5, tmp1);
	sprintf(m_cLineTemp, "$BCTEMP,%i,%s,C*",
		sensorId,
		tmp1);
	PrintSentence();
}

/// <summary>
/// Writes the humidity temperature of DHT11.
/// </summary>
/// <param name="iDigitalPort">The i digital port.</param>
void bcplanet_dsb::WriteHumidityTemperatureDHT11(int iDigitalPort)
{
	int chk = DHT.read11(iDigitalPort);//read the value returned from sensor
	char tmp1[20];
	switch (chk)
	{
	case DHTLIB_OK:

		WriteTemperatureSentence(iDigitalPort, DHT.temperature);
		WriteHumiditySentence(iDigitalPort, DHT.humidity);

		break;
	case DHTLIB_ERROR_CHECKSUM:
		//Serial.print("Checksum error,\t"); 
		WriteError(iDigitalPort, 0, "Checksum error");
		break;
	case DHTLIB_ERROR_TIMEOUT:
		//Serial.print("Time out error,\t"); 
		WriteError(iDigitalPort, 0, "Time out humidity/temperature sensor error");
		break;
	default:
		//Serial.print("Unknown error,\t"); 
		WriteError(iDigitalPort, 0, "Unknown humidity/temperature sensor error");
		break;
	}
}

// GAS
/*
* convert an adc value to a resistence value
*/
long bcplanet_dsb::adc_getresistence(int adcread, int adcbalanceresistor)
{
	if (adcread == 0)
		return 0;
	return ((ADC_REFRES*adcbalanceresistor) / adcread - adcbalanceresistor);
}

/*
* get the calibrated ro based upon read resistance, and a know ppm
*/
long bcplanet_dsb::mq135_getro(long resvalue, double ppm) 
{
	return static_cast<long>(resvalue * exp(log(MQ135_SCALINGFACTOR / ppm) / MQ135_EXPONENT));
}

/*
* get the ppm concentration
*/
double bcplanet_dsb::mq135_getppm(long resvalue, long ro)
{
	double ret = 0;
	double validinterval = 0;
	validinterval = resvalue / static_cast<double>(ro);
	//if (validinterval<MQ135_MAXRSRO && validinterval>MQ135_MINRSRO) {
	ret = static_cast<double>(MQ135_SCALINGFACTOR) * pow((static_cast<double>(resvalue) / ro), MQ135_EXPONENT);
	//}
	return ret;
}

void bcplanet_dsb::WriteGas(int sensorId, int analog, double correction)
{
	long sum = 0;
	for (auto i = 0; i < 100; i++)
	{
		sum = sum + analogRead(analog);
	}
	int adc = (int)((double)sum / 100.0);

	MQ135_DEFAULTPPM = 406;// 392; //default ppm of CO2 for calibration
	MQ135_DEFAULTRO = 41763; //default Ro for MQ135_DEFAULTPPM ppm of CO2
	MQ135_SCALINGFACTOR = 116.6020682; // 56.0820; //CO2 gas value
	MQ135_EXPONENT = -2.769034857; // -5.9603; //CO2 gas value
	ADC_REFRES = 1024;

	//calculated resistence depends on the sensor pulldown resistor
	long res = adc_getresistence(adc, MQ135_PULLDOWNRES); // 26954;
															  //get ro
	long mq135_ro = mq135_getro(res, MQ135_DEFAULTPPM);
	//convert to ppm (using default ro)
	double d = mq135_getppm(res, MQ135_DEFAULTRO);

	char tmp1[20];
	char tmp2[20];
	char tmp3[20];
	char tmp4[20];
	itoa(adc, tmp1, 10);
	ltoa(res, tmp2, 10);
	ltoa(mq135_ro, tmp3, 10);
	dtostrf(d + correction, 3, 5, tmp4);

	sprintf(m_cLineTemp, "$BCGAS,%i,%s,%s,%s,%s,ppm*",
		sensorId,
		tmp1,
		tmp2,
		tmp3,
		tmp4);
	PrintSentence();
}

void bcplanet_dsb::WritePir(int sensorId)
{
	if (digitalRead(PIR_PIN) == HIGH) 
	{
		digitalWrite(13, HIGH);   //the led visualizes the sensors output pin state
		if (lockLow)
		{
			//makes sure we wait for a transition to LOW before any further output is made:
			lockLow = false;
			/*Serial.println("---");
			Serial.print("motion detected at ");
			Serial.print(millis() / 1000);
			Serial.println(" sec");*/
		
			sprintf(m_cLineTemp, "$BCPIR,%i,1*", sensorId);
			PrintSentence();
			delay(50);
		}
		takeLowTime = true;
	}

	if (digitalRead(PIR_PIN) == LOW)
	{
		digitalWrite(13, LOW);  //the led visualizes the sensors output pin state

		if (takeLowTime) 
		{
			lowIn = millis();          //save the time of the transition from high to LOW
			takeLowTime = false;       //make sure this is only done at the start of a LOW phase
		}
		//if the sensor is low for more than the given pause, 
		//we assume that no more motion is going to happen
		if (!lockLow && millis() - lowIn > pause) {
			//makes sure this block of code is only executed again after 
			//a new motion sequence has been detected
			lockLow = true;
			//Serial.print("motion ended at ");      //output
			//Serial.print((millis() - pause) / 1000);
			//Serial.println(" sec");

			sprintf(m_cLineTemp, "$BCPIR,%i,0*", sensorId);
			PrintSentence();
			delay(50);
		}
	}
}