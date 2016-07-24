#include <OneWire.h>
#include "bcplanet_dsb.h"


//---------------------------------
// NMEA
//#define MAXLINELENGTH 150
//static char m_cLineTemp[MAXLINELENGTH];
//static char m_cLinePrint[MAXLINELENGTH];
//static String m_sSentence;

OneWire  oneWire(ONEWIRE_PIN);


//#define MQ135_PULLDOWNRES 3150//1450//1450
//
//long ADC_REFRES = 1024; //reference resolution used for conversions
//
//double MQ135_DEFAULTPPM = 404; //default ppm of CO2 for calibration
//double MQ135_DEFAULTRO = 41763; //default Ro for MQ135_DEFAULTPPM ppm of CO2
//double MQ135_SCALINGFACTOR = 116.6020682; //CO2 gas value
//double MQ135_EXPONENT = -2.769034857; //CO2 gas value
//double MQ135_MAXRSRO = 2.428; //for CO2
//double MQ135_MINRSRO = 0.358; //for CO2

//char printbuff[100];
//double d = 0;
//int adc = 0;
//
//long mq135_ro = 0;
//long sum = 0;

bcplanet_dsb DSB;
//dht DHT;//create a variable type of dht
//const int DHT11_PIN = 7;//Humiture sensor attach to pin7

//#define beta 4090 //the beta of the thermistor


void setup()
{
#if defined (__AVR_ATmega32U4__) // no ASSR register for Arduino Leonardo
	while (!Serial)
	{
		; // wait for serial port to connect. Needed for Leonardo only
	}
#endif
	Serial.begin(115200);

	DSB.SetOneWireReference(&oneWire);
}

void loop()
{

	DSB.WriteGas(20, 3, 307);
	DSB.WriteGas(21, 0, 400.2);
	DSB.WritePir(30);
	DSB.WriteFreeRam(0);
	DSB.WriteProgramSize(0);
	DSB.WriteHumidityTemperatureDHT11(8);
	DSB.WriteAnalogPlain(0);
	DSB.WriteAnalog(0);

	DSB.WriteTemperatureDS18B20(2);
	//delay(1000);
}


