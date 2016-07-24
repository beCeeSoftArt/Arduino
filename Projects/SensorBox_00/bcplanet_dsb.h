// bcplanet_dsb.h

#ifndef _BCPLANET_DSB_h
#define _BCPLANET_DSB_h

#include <dht.h>
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
	void WriteError(int isensorId, int errorId, char* pMessage);
	void WriteAnalog(int isensorId);
	void WriteAnalogPlain(int isensorId);
	void WriteHumidityTemperatureDHT11(int iDigitalPort);
	void WriteGas(int sensorId, int analog, double correction);

	void WritePir(int sensorId);

	void SetOneWireReference(OneWire *oneWireRef);
	void WriteTemperatureDS18B20(int sensorId);

protected:
	dht DHT;
	OneWire *oneWire;
	
	double MQ135_DEFAULTPPM; //default ppm of CO2 for calibration
	double MQ135_DEFAULTRO; //default Ro for MQ135_DEFAULTPPM ppm of CO2
	double MQ135_SCALINGFACTOR; //CO2 gas value
	double MQ135_EXPONENT; //CO2 gas value
	double MQ135_MAXRSRO; //for CO2
	double MQ135_MINRSRO; //for CO2
	long ADC_REFRES = 1024; //reference resolution used for conversions

	//PIR the time when the sensor outputs a low impulse
	long unsigned int lowIn;

	//PIR the amount of milliseconds the sensor has to be low 
	//before we assume all motion has stopped
	long unsigned int pause = 5000;
	//PIR 
	bool lockLow = true;
	bool takeLowTime;
	
	void PrintSentence();
	int CreateCheckSum(char *pNMEA);
	int FreeRam();
	unsigned long ProgramSize();
	float ConvertToVoltage(float fValue);
	void WriteHumiditySentence(int sensorId, float sensorValue);
	void WriteTemperatureSentence(int sensorId, float sensorValue);
	
	long adc_getresistence(int adcread, int adcbalanceresistor);
	long mq135_getro(long resvalue, double ppm);
	double mq135_getppm(long resvalue, long ro);
private:
	
};


#endif

