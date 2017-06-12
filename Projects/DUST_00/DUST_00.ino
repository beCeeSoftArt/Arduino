// NMEA
#define MAXLINELENGTH 150
static char m_cLineTemp[MAXLINELENGTH];
static char m_cLinePrint[MAXLINELENGTH];
static String m_sSentence;
void PrintSentence();
int CreateCheckSum(char *pNMEA);
void WriteDust(int iSensorId, float value);


int pin = 8;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 5000;//sampe 30s&nbsp;;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
float lastValue = 0;

void setup() 
{
	Serial.begin(115200);
	pinMode(8, INPUT);
	starttime = millis();//get the current time;
}

void loop() 
{
	duration = pulseIn(pin, LOW);
	lowpulseoccupancy = lowpulseoccupancy + duration;

	if ((millis() - starttime) >= sampletime_ms)//if the sampel time = = 30s
	{
		ratio = lowpulseoccupancy / (sampletime_ms*10.0);  // Integer percentage 0=&gt;100
		concentration = 1.1*pow(ratio, 3) - 3.8*pow(ratio, 2) + 520 * ratio + 0.62; // using spec sheet curve
		/*Serial.print("concentration = ");
		Serial.print(concentration);
		Serial.println(" pcs/0.01cf");*/

		lastValue = lastValue * 0.9 + concentration * 0.1;

		WriteDust(0, lastValue);
		lowpulseoccupancy = 0;
		starttime = millis();
	}
}

void WriteDust(int iSensorId, float value)
{
	char tmp1[10];
	dtostrf(value, 3, 2, tmp1);
	sprintf(m_cLineTemp, "$BCDUST,%i,%s,0.01cf*", iSensorId, tmp1);
	PrintSentence();
}
// ----------------------------
// NMEA

/// <summary>
/// Sends the sentence and adds a checksum.
/// </summary>
void PrintSentence()
{
	// Add NMEA checksum as HEX string
	sprintf(m_cLinePrint, "%X", CreateCheckSum(m_cLineTemp));

	m_sSentence = m_cLineTemp;
	m_sSentence += m_cLinePrint;
	Serial.println(m_sSentence);
}

/// <summary>
/// Creates the check sum.
/// </summary>
/// <param name="pNMEA">The nmea sentence.</param>
/// <returns>The checksum</returns>
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