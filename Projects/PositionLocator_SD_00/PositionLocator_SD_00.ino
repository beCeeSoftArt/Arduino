#include <SPI.h>
#include <SD.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

/*
   This sample sketch demonstrates the normal use of a TinyGPSPlus (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const int RXPin = 7, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

//Sd2Card card;
//SdVolume volume;
//SdFile root;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN
const int chipSelect = 10;
File dataFile;

void displayInfo();
void AppendGpsDataToSDCard();

void setup()
{
	pinMode(6, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);

	// Open serial communications and wait for port to open:
	Serial.begin(115200);
	while (!Serial) 
	{
		// wait for serial port to connect. Needed for native USB port only
	}

	//// we'll use the initialization code from the utility libraries
	//// since we're just testing if the card is working!
	//if (!card.init(SPI_HALF_SPEED, chipSelect)) 
	//{
	//	Serial.println("initialization failed. Things to check:");
	//	Serial.println("* is a card inserted?");
	//	Serial.println("* is your wiring correct?");
	//	Serial.println("* did you change the chipSelect pin to match your shield or module?");
	//	while (1);
	//}
	//else 
	//{
	//	Serial.println("Wiring is correct and a card is present.");
	//}

	//// print the type of card
	//Serial.println();
	//Serial.print("Card type:         ");
	//switch (card.type())
	//{
	//	case SD_CARD_TYPE_SD1:
	//		Serial.println("SD1");
	//		break;
	//	case SD_CARD_TYPE_SD2:
	//		Serial.println("SD2");
	//		break;
	//	case SD_CARD_TYPE_SDHC:
	//		Serial.println("SDHC");
	//		break;
	//	default:
	//		Serial.println("Unknown");
	//}

	//// Try to open the 'volume'/'partition' - it should be FAT16 or FAT32
	//if (!volume.init(card)) 
	//{
	//	Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
	//	while (true)
	//	{
	//		digitalWrite(6, LOW);
	//		delay(500);
	//		digitalWrite(6, HIGH);
	//		delay(500);
	//	}
	//}

	Serial.print("Initializing SD card...");

	if (!SD.begin(chipSelect))
	{
		Serial.println("initialization failed!");
		while (true)
		{
			digitalWrite(6, LOW);
			delay(500);
			digitalWrite(6, HIGH);
			delay(500);
		}
	}
	Serial.println("initialization done.");
	digitalWrite(6, HIGH);
	delay(500);
	digitalWrite(6, LOW);

	// Start software serial
	ss.begin(GPSBaud);
	
	
	digitalWrite(8, LOW);
	digitalWrite(9, LOW);
}

void loop()
{
	// This sketch displays information every time a new sentence is correctly encoded.
	while (ss.available() > 0)
		if (gps.encode(ss.read()))
			displayInfo();

	if (millis() > 5000 && gps.charsProcessed() < 10)
	{
		Serial.println(F("No GPS detected: check wiring."));
		while (true);
	}
}

void displayInfo()
{
	Serial.print(F("Location: "));
	if (gps.location.isValid())
	{
		Serial.print(gps.location.lat(), 6);
		Serial.print(F(","));
		Serial.print(gps.location.lng(), 6);
	}
	else
	{
		Serial.print(F("INVALID"));
	}

	Serial.print(F("  Date/Time: "));
	if (gps.date.isValid())
	{
		Serial.print(gps.date.month());
		Serial.print(F("/"));
		Serial.print(gps.date.day());
		Serial.print(F("/"));
		Serial.print(gps.date.year());
	}
	else
	{
		Serial.print(F("INVALID"));
	}

	Serial.print(F(" "));
	if (gps.time.isValid())
	{
		if (gps.time.hour() < 10) 
			Serial.print(F("0"));
		Serial.print(gps.time.hour());
		Serial.print(F(":"));
		if (gps.time.minute() < 10) 
			Serial.print(F("0"));
		Serial.print(gps.time.minute());
		Serial.print(F(":"));
		if (gps.time.second() < 10) 
			Serial.print(F("0"));
		Serial.print(gps.time.second());
		Serial.print(F("."));
		if (gps.time.centisecond() < 10) 
			Serial.print(F("0"));
		Serial.print(gps.time.centisecond());
	}
	else
	{
		Serial.print(F("INVALID"));
	}

	Serial.println();

	if (gps.time.isValid())
	{
		digitalWrite(8, HIGH);
		digitalWrite(9, LOW);

		

	}
	else
	{
		digitalWrite(8, LOW);
		digitalWrite(9, HIGH);

	}
	AppendGpsDataToSDCard();

	delay(1000);
}

void AppendGpsDataToSDCard()
{
	digitalWrite(6, HIGH);

	String fileName = String();

	fileName += gps.date.year();
	if (gps.date.month() < 10)
		fileName += "0";
	fileName += gps.date.month();
	if (gps.date.day() < 10)
		fileName += "0";
	fileName += gps.date.day();
	fileName += ".txt";

	// open the file. note that only one file can be open at a time,
	// so you have to close this one before opening another.
	dataFile = SD.open(fileName, FILE_WRITE);

	// if the file opened okay, write to it:
	if (dataFile) 
	{		
		// Write DateTime
		dataFile.print(gps.date.year());
		dataFile.print(".");
		if (gps.date.month() < 10)
			dataFile.print("0");
		dataFile.print(gps.date.month());
		dataFile.print(".");
		if (gps.date.day() < 10)
			dataFile.print("0");
		dataFile.print(gps.date.day());
		dataFile.print(" ");
		if (gps.time.hour() < 10)
			dataFile.print("0");
		dataFile.print(gps.time.hour());
		dataFile.print(":");
		if (gps.time.minute() < 10)
			dataFile.print("0");
		dataFile.print(gps.time.minute());
		dataFile.print(":");
		if (gps.time.second() < 10)
			dataFile.print("0");
		dataFile.print(gps.time.second());

		// Separator
		dataFile.print(";");

		// Location
		dataFile.print(gps.location.lat());
		dataFile.print(";");
		dataFile.println(gps.location.lng());
		
		// close the file:
		dataFile.close();

		Serial.print("Write to file: ");
		Serial.println(fileName);
	}
	else 
	{
		Serial.print("Error opening file: ");
		Serial.println(fileName);
	}
	
	digitalWrite(6, LOW);
}