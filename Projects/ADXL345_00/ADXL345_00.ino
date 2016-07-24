#include <Wire.h>
#include <ADXL345.h>

//---------------------------------
// NMEA
#define MAXLINELENGTH 150
static char m_cLineTemp[MAXLINELENGTH];
static char m_cLinePrint[MAXLINELENGTH];
static String m_sSentence;
//---------------------------------

ADXL345 accel;

// Set up a pin we are going to use to indicate our status using an LED.
int statusPin = 13; // I'm using digital pin 2.

void setup()
{
	// Begin by setting up the Serial Port so we can output our results.
	Serial.begin(115200);
	// Start the I2C Wire library so we can use I2C to talk to the accelerometer.
	Wire.begin();

	// Ready an LED to indicate our status.
	pinMode(statusPin, OUTPUT);

	// Create an instance of the accelerometer on the default address (0x1D)
	accel = ADXL345();

	// Check that the accelerometer is infact connected.
	if (accel.EnsureConnected())
	{
		Serial.println("Connected to ADXL345.");
		digitalWrite(statusPin, HIGH); // If we are connected, light our status LED.
	}
	else
	{
		Serial.println("Could not connect to ADXL345.");
		digitalWrite(statusPin, LOW); // If we are not connected, turn our LED off.
	}

	// Set the range of the accelerometer to a maximum of 2G.
	accel.SetRange(2, true);
	// Tell the accelerometer to start taking measurements.
	accel.EnableMeasurements();
}

void loop()
{
	if (accel.IsConnected) // If we are connected to the accelerometer.
	{
		// Read the raw data from the accelerometer.
		//AccelerometerRaw raw = accel.ReadRawAxis();
		//This data can be accessed like so:
		//int xAxisRawData = raw.XAxis;

		// Read the *scaled* data from the accelerometer (this does it's own read from the accelerometer
		// so you don't have to ReadRawAxis before you use this method).
		// This useful method gives you the value in G thanks to the Love Electronics library.
		//AccelerometerScaled scaled = accel.ReadScaledAxis();
		// This data can be accessed like so:
		//float xAxisGs = scaled.XAxis;

		// We output our received data.
		//Output(raw, scaled);

		WriteAccel(9, accel.ReadRawAxis(), accel.ReadScaledAxis());
	}
	delay(250);
}

// Output the data down the serial port.
void Output(AccelerometerRaw raw, AccelerometerScaled scaled)
{
	// Tell us about the raw values coming from the accelerometer.
	Serial.print("Raw:\t");
	Serial.print(raw.XAxis);
	Serial.print("   ");
	Serial.print(raw.YAxis);
	Serial.print("   ");
	Serial.print(raw.ZAxis);

	// Tell us about the this data, but scale it into useful units (G).
	Serial.print("   \tScaled:\t");
	Serial.print(scaled.XAxis);
	Serial.print("G   ");
	Serial.print(scaled.YAxis);
	Serial.print("G   ");
	Serial.print(scaled.ZAxis);
	Serial.println("G");
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


void WriteAccel(int iSensorId, AccelerometerRaw raw, AccelerometerScaled scaled)
{
	char tmp1[20];
	char tmp2[20];
	char tmp3[20];
	char tmp4[20];
	char tmp5[20];
	char tmp6[20];
	itoa(raw.XAxis, tmp1, 10);
	itoa(raw.YAxis, tmp2, 10);
	itoa(raw.ZAxis, tmp3, 10);
	dtostrf(scaled.XAxis, 3, 5, tmp4);
	dtostrf(scaled.YAxis, 3, 5, tmp5);
	dtostrf(scaled.ZAxis, 3, 5, tmp6);
	sprintf(m_cLineTemp, "$BCACCEL,%i,%s,%s,%s,%s,%s,%s*",
		iSensorId,
		tmp1,
		tmp2,
		tmp3,
		tmp4,
		tmp5,
		tmp6);
	PrintSentence();
}
