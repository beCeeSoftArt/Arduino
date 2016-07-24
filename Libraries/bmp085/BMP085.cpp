/*
	Copyright André Spitzner, 2003 - 2012

	BMP085 Pressure Sensor Implementation
*/
// Includes
extern "C"
{
	#include <stdlib.h>
	#include <string.h>
	#include <inttypes.h>
}

#include <Arduino.h>
#include "Wire.h"
#include "BMP085.h"

// Initialize Class Variables /////////////////////////////////////////////////
/*
	Initialize Default Calibration Values
*/
int BMP085::ac1;
int BMP085::ac2;
int BMP085::ac3;
unsigned int BMP085::ac4;
unsigned int BMP085::ac5;
unsigned int BMP085::ac6;
int BMP085::b1;
int BMP085::b2;
int BMP085::mb;
int BMP085::mc;
int BMP085::md;
long BMP085::b5;

// Constructors ////////////////////////////////////////////////////////////////
/*
	Base Constructor
*/
BMP085::BMP085()
{
	// Start I2C
	Wire.begin();
}

// Public Methods //////////////////////////////////////////////////////////////

/// <summary>
/// Stores all of the bmp085's calibration values into variables
/// Calibration values are required to calculate temp and pressure
/// This function should be called at the beginning of the program
/// </summary>
void BMP085::CalibrateSensor()
{
	ac1 = ReadDeviceInt(0xAA);
	ac2 = ReadDeviceInt(0xAC);
	ac3 = ReadDeviceInt(0xAE);
	ac4 = ReadDeviceInt(0xB0);
	ac5 = ReadDeviceInt(0xB2);
	ac6 = ReadDeviceInt(0xB4);
	b1 = ReadDeviceInt(0xB6);
	b2 = ReadDeviceInt(0xB8);
	mb = ReadDeviceInt(0xBA);
	mc = ReadDeviceInt(0xBC);
	md = ReadDeviceInt(0xBE);
}

/// <summary>
/// Calculate temperature in deg C
/// </summary>
/// <param name="iUT">The i UT.</param>
/// <returns></returns>
float BMP085::GetTemperature(unsigned int iUT)
{
	long x1, x2;

	x1 = (((long)iUT - (long)ac6)*(long)ac5) >> 15;
	x2 = ((long)mc << 11)/(x1 + md);
	b5 = x1 + x2;

	float temp = ((b5 + 8)>>4);
	temp = temp /10;

	return temp;
}

/// <summary>
/// Calculate pressure given up calibration values must be known
/// b5 is also required so bmp085GetTemperature(...) must be called first.
/// Value returned will be pressure in units of Pa.
/// </summary>
/// <param name="iUP">The i UP.</param>
/// <returns></returns>
long BMP085::GetPressure(unsigned long iUP)
{
	long x1, x2, x3, b3, b6, p;
	unsigned long b4, b7;

	b6 = b5 - 4000;
	// Calculate B3
	x1 = (b2 * (b6 * b6)>>12)>>11;
	x2 = (ac2 * b6)>>11;
	x3 = x1 + x2;
	b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;

	// Calculate B4
	x1 = (ac3 * b6)>>13;
	x2 = (b1 * ((b6 * b6)>>12))>>16;
	x3 = ((x1 + x2) + 2)>>2;
	b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

	b7 = ((unsigned long)(iUP - b3) * (50000>>OSS));
	if (b7 < 0x80000000)
	p = (b7<<1)/b4;
	else
	p = (b7/b4)<<1;

	x1 = (p>>8) * (p>>8);
	x1 = (x1 * 3038)>>16;
	x2 = (-7357 * p)>>16;
	p += (x1 + x2 + 3791)>>4;

	long temp = p;
	return temp;
}


/// <summary>
/// Read 1 byte from the BMP085 at 'address'
/// </summary>
/// <param name="bAddress">The address.</param>
/// <returns></returns>
char BMP085::ReadDevice(byte bAddress)
{
	Wire.beginTransmission(BMP085_ADDRESS);
	Wire.write(bAddress);
	Wire.endTransmission();
	Wire.requestFrom(BMP085_ADDRESS, 1);
	while(!Wire.available()) {};
	return Wire.read();
}

/// <summary>
/// Read 2 bytes from the BMP085, 
/// First byte will be from 'address', 
/// Second byte will be from 'address'+1
/// </summary>
/// <param name="bAddress">The b address.</param>
/// <returns></returns>
int BMP085::ReadDeviceInt(byte bAddress)
{
	unsigned char msb, lsb;

	Wire.beginTransmission(BMP085_ADDRESS);
	Wire.write(bAddress);
	Wire.endTransmission();

	Wire.requestFrom(BMP085_ADDRESS, 2);
	while(Wire.available()<2)
	;
	msb = Wire.read();
	lsb = Wire.read();

	return (int) msb<<8 | lsb;
}


/// <summary>
/// Read the uncompensated temperature value.
/// </summary>
/// <returns></returns>
unsigned int BMP085::ReadDeviceUT()
{
	unsigned int iUT;

	// Write 0x2E into Register 0xF4
	// This requests a temperature reading
	Wire.beginTransmission(BMP085_ADDRESS);
	Wire.write((byte)0xF4);
	Wire.write((byte)0x2E);
	Wire.endTransmission();

	// Wait at least 4.5ms
	delay(5);

	// Read two bytes from registers 0xF6 and 0xF7
	iUT = ReadDeviceInt(0xF6);
	return iUT;
}

/// <summary>
/// Read the uncompensated pressure value
/// </summary>
/// <returns></returns>
unsigned long BMP085::ReadDeviceUP()
{

	unsigned char msb, lsb, xlsb;
	unsigned long iUP = 0;

	// Write 0x34+(OSS<<6) into register 0xF4
	// Request a pressure reading w/ oversampling setting
	Wire.beginTransmission(BMP085_ADDRESS);
	Wire.write(0xF4);
	Wire.write(0x34 + (OSS<<6));
	Wire.endTransmission();

	// Wait for conversion, delay time dependent on OSS
	delay(2 + (3<<OSS));

	// Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
	msb = ReadDevice(0xF6);
	lsb = ReadDevice(0xF7);
	xlsb = ReadDevice(0xF8);

	iUP = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);

	return iUP;
}

/// <summary>
/// Writes a value to the device register.
/// </summary>
/// <param name="iDeviceAddress">The i device address.</param>
/// <param name="bAddress">The b address.</param>
/// <param name="bValue">The b value.</param>
void BMP085::WriteDeviceRegister(int iDeviceAddress, byte bAddress, byte bValue) 
{
	Wire.beginTransmission(iDeviceAddress); // start transmission to device
	Wire.write(bAddress);       // send register address
	Wire.write(bValue);         // send value to write
	Wire.endTransmission();     // end transmission
}

/// <summary>
/// Reads the a value from a device register.
/// </summary>
/// <param name="iDeviceAddress">The i device address.</param>
/// <param name="bAddress">The b address.</param>
/// <returns></returns>
int BMP085::ReadDeviceRegister(int iDeviceAddress, byte bAddress)
{
	int iValue;
	Wire.beginTransmission(iDeviceAddress);
	Wire.write(bAddress); // register to read
	Wire.endTransmission();

	Wire.requestFrom(iDeviceAddress, 1); // read a byte

	while(!Wire.available()) 
	{
		// waiting
	}

	iValue = Wire.read();
	return iValue;
}

/// <summary>
/// Calculates the altitude.
/// </summary>
/// <param name="fPressure">The f pressure.</param>
/// <returns></returns>
float BMP085::CalculateAltitude(float fPressure)
{
	float fA = fPressure/101325;
	float fB = 1/5.25588;
	float fC = pow(fA,fB);
	fC = 1 - fC;
	fC = fC /0.0000225577;

	return fC;
}