#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <stdio.h>
#include <math.h>

// Connect VCC of the BMP085 sensor to 3.3V (NOT 5.0V!)
// Connect GND to Ground
// Connect SCL to i2c clock - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 5
// Connect SDA to i2c data - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 4
// EOC is not used, it signifies an end of conversion
// XCLR is a reset pin, also not used here
//---------------------------------
// NMEA
#define MAXLINELENGTH 150
static char m_cLineTemp[MAXLINELENGTH];
static char m_cLinePrint[MAXLINELENGTH];
static String m_sSentence;




Adafruit_BMP085 bmp;

void setup() {
	Serial.begin(115200);
	if (!bmp.begin()) {
		Serial.println("Could not find a valid BMP085 sensor, check wiring!");
		while (1) {}
	}
}

void loop() {
	//Serial.print("Temperature = ");
	//Serial.print(bmp.readTemperature());
	//Serial.println(" *C");

	//Serial.print("Pressure = ");
	//Serial.print(bmp.readPressure());
	//Serial.println(" Pa");

	//// Calculate altitude assuming 'standard' barometric
	//// pressure of 1013.25 millibar = 101325 Pascal
	//Serial.print("Altitude = ");
	//Serial.print(bmp.readAltitude());
	//Serial.println(" meters");

	//Serial.print("Pressure at sealevel (calculated) = ");
	//Serial.print(bmp.readSealevelPressure());
	//Serial.println(" Pa");

	//// you can get a more precise measurement of altitude
	//// if you know the current sea level pressure which will
	//// vary with weather and such. If it is 1015 millibars
	//// that is equal to 101500 Pascals.
	//Serial.print("Real altitude = ");
	//Serial.print(bmp.readAltitude(101500));
	//Serial.println(" meters");

	//Serial.println();

	WriteTemperature(8);
	WriteAltitude(8);
	WritePressure(8);
	WriteFreeRam(1);
	WriteProgramSize(1);
	WriteAnalog(1);
	WriteAnalogPlain(1);

	delay(1000);
}

//----------------------------
// NMEA

/// <summary>
/// Prints the sentence.
/// </summary>
void PrintSentence()
{
	sprintf(m_cLinePrint, "%X", CreateCheckSum(m_cLineTemp));

	m_sSentence = m_cLineTemp;
	m_sSentence += m_cLinePrint;
	Serial.println(m_sSentence);
}

/// <summary>
/// Creates the check sum.
/// </summary>
/// <param name="pNMEA">The p nmea.</param>
/// <returns></returns>
int CreateCheckSum(char *pNMEA)
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

float ConvertToVoltage(float fValue)
{
	float fRet = fValue * (5.0 / 1023.0);
	return (fRet);
}

int FreeRam()
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
unsigned long ProgramSize()
{
	extern int _etext;
	extern int _edata;

	return (reinterpret_cast<unsigned long>(&_etext) + (reinterpret_cast<unsigned long>(&_edata) - 256L));
}

void WriteTemperature(int iAnalogId)
{
	char tmp1[20];
	dtostrf(bmp.readTemperature(), 3, 5, tmp1);
	sprintf(m_cLineTemp, "$BCTEMP,%i,%s,C*",
		iAnalogId,
		tmp1);
	PrintSentence();
}

void WriteAltitude(int iAnalogId)
{
	char tmp1[20];
	dtostrf(bmp.readAltitude(101500), 3, 5, tmp1);
	sprintf(m_cLineTemp, "$BCALTITUDE,%i,%s,m*",
		iAnalogId,
		tmp1);
	PrintSentence();
}

void WritePressure(int iAnalogId)
{
	char tmp1[20];
	dtostrf(bmp.readPressure()/100000.0, 3, 5, tmp1);
	sprintf(m_cLineTemp, "$BCPRESSURE,%i,%s,Bar*",
		iAnalogId,
		tmp1);
	PrintSentence();
}

void WriteFreeRam(int sensorId)
{
	sprintf(m_cLineTemp, "$BCFREERAM,%i,%i,Byte*",
		sensorId,
		FreeRam());
	PrintSentence();
}

/// <summary>
/// Writes the current program size.
/// </summary>
void WriteProgramSize(int sensorId)
{
	sprintf(m_cLineTemp, "$BCPROGSIZE,%i,%i,Byte*",
		sensorId,
		ProgramSize());
	PrintSentence();
}

void WriteAnalog(int iSensorId)
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
		iSensorId,
		tmp1,
		tmp2,
		tmp3,
		tmp4,
		tmp5,
		tmp6);
	PrintSentence();
}

void WriteAnalogPlain(int iSensorId)
{
	sprintf(m_cLineTemp, "$BCANALOGPLAIN,%i,%i,%i,%i,%i,%i,%i*",
		iSensorId,
		analogRead(A0),
		analogRead(A1),
		analogRead(A2),
		analogRead(A3),
		analogRead(A4),
		analogRead(A5));
	PrintSentence();
}
