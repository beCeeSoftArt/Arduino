/*
	Copyright André Spitzner, 2003 - 2012
	
	BMP085 Pressure Sensor Implementation
 */ 


#ifndef BMP085_H_
#define BMP085_H_

// I2C address of BMP085
#define BMP085_ADDRESS 0x77  
// Oversampling Setting
const unsigned char OSS = 2;  

/*
	BMP085 Pressure Sensor Class Definition
*/
class BMP085
{
	private:
		// Calibration Values
		static int ac1;
		static int ac2;
		static int ac3;
		static unsigned int ac4;
		static unsigned int ac5;
		static unsigned int ac6;
		static int b1;
		static int b2;
		static int mb;
		static int mc;
		static int md;

		// b5 is calculated in bmp085GetTemperature(...), this variable is also used in bmp085GetPressure(...)
		// so ...Temperature(...) must be called before ...Pressure(...).
		static long b5;
	public:
		// Base Constructor
		BMP085();
		void CalibrateSensor();
		float GetTemperature(unsigned int ut);
		long GetPressure(unsigned long up);
		char ReadDevice(byte address);
		int ReadDeviceInt(byte address);
		unsigned int ReadDeviceUT();
		unsigned long ReadDeviceUP();
		void WriteDeviceRegister(int deviceAddress, byte address, byte val);
		int ReadDeviceRegister(int deviceAddress, byte address);
		float CalculateAltitude(float pressure);
};


/*
	Already Usage Instance
*/
extern BMP085 oBMP085;



#endif /* BMP085_H_ */