#include <TB6612FNG.h>

TB6612FNG engine = TB6612FNG(3, 9, 8, 5, 11, 12);

void setup()
{
#if defined (__AVR_ATmega32U4__) // No ASSR Register For Arduino Leonardo
	while (!Serial)
	{
		; // Wait For Serial Port To Connect.
	}
#endif

	pinMode(13, OUTPUT);
}


void loop()
{
	digitalWrite(13, HIGH);

	for (int i = 1; i < 26; i++)
	{
		engine.EnginePower(0, 1, i * 20);
		engine.EnginePower(1, 1, i * 20);

		delay(1000);
	}

	engine.Standby();

	for (int i = 25; i > 0; i--)
	{
		engine.EnginePower(0, 1, i * 20);
		engine.EnginePower(1, 1, i * 20);

		delay(1000);
	}

	delay(2000);


	digitalWrite(13, LOW);

	engine.Standby();

	delay(2000);

}
