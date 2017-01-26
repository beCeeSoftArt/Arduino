// bcplanet_dsb.h

#ifndef _BCPLANET_DSB_h
#define _BCPLANET_DSB_h

#include <dht.h>
#include <StopWatch.h>
#include <stdio.h>
#include <math.h>
#include <OneWire.h>

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
//---------------------------------
// NMEA
#define MAXLINELENGTH 150
static char m_cLineTemp[MAXLINELENGTH];
static char m_cLinePrint[MAXLINELENGTH];
static String m_sSentence;

#define MQ135_PULLDOWNRES 3150//1450//1450

const int DHT11_PIN = 8;
const int ONEWIRE_PIN = 2;
const int PIR_PIN = 9;


class bcplanet_dsb
{
public:
	bcplanet_dsb();
	
	void WriteFreeRam(int sensorId);
	void WriteProgramSize(int sensorId);
	void WriteError(int sensorId, int errorId, char* pMessage);
	void WriteAnalog(int sensorId);
	void WriteAnalogPlain(int sensorId);
	void WriteHumidityTemperatureDHT(int digitalPort, int sensorType, double correctionT, double correctionRH);
	void WriteGas(int sensorId, int analog, double correction, char* pGasName);

	void WritePir(int sensorId);

	void SetOneWireReference(OneWire *oneWireRef);
	void WriteTemperatureDS18B20();
	
	void StartElapsedTime();
	void WriteElapsedTime(int sensorId);

	void WriteHumidityHIH4030(int analogId, double correction);

protected:
	int iRange;
	dht DHT;
	OneWire *oneWire;
	// Stopwatch
	StopWatch oStopWatch;

	// Default ppm of gas for calibration
	double MQ135_DEFAULTPPM;
	// Default Ro for MQ135_DEFAULTPPM ppm of gas
	double MQ135_DEFAULTRO; 
	// Gas value scaling factor
	double MQ135_SCALINGFACTOR; 
	// Gas value
	double MQ135_EXPONENT;
	// Reference resolution used for conversions
	long ADC_REFRES = 1024; 

	// PIR the time when the sensor outputs a low impulse
	long unsigned int lowIn;

	// PIR the amount of milliseconds the sensor has to be low 
	// before we assume all motion has stopped
	long unsigned int pause = 5000;

	// PIR 
	bool lockLow = true;
	// Take low time
	bool takeLowTime;
	
	// Writes NMEA sentence to the serial port
	void PrintSentence();
	// Create NMEA check sum
	int CreateCheckSum(char *pNMEA);

	// Get free RAM
	int FreeRam();
	// Get programm size
	unsigned long ProgramSize();
	// Convert analog value to voltage
	float ConvertToVoltage(float fValue);
	
	// Internal write method used for DHT*
	void WriteHumiditySentence(int sensorId, float sensorValue);
	// Internal write method used for DHT*
	void WriteTemperatureSentence(int sensorId, float sensorValue);

	// Get gas sensor resistence value
	long adc_getresistence(int adcread, int adcbalanceresistor);
	// Get gas sensor Ro  value
	long mq135_getro(long resvalue, double ppm);
	// Calculate ppm value
	double mq135_getppm(long resvalue, long ro);

	// Get humidity from analog port
	float GetHumidity(int analogPort);

private:
	
};


#endif

