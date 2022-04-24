#include <Wire.h>
#include "Kalman.h"
#include <StepperMotor.h>
#include <LIDARLite.h>

// I2C -------------------------------------------
const uint8_t IMUAddress = 0x68; // AD0 is logic low on the PCB
const uint16_t I2C_TIMEOUT = 1000; // Used to check for errors in I2C communication

uint8_t i2cWrite(uint8_t registerAddress, uint8_t data, bool sendStop)
{
	return i2cWrite(registerAddress, &data, 1, sendStop); // Returns 0 on success
}

uint8_t i2cWrite(uint8_t registerAddress, uint8_t* data, uint8_t length, bool sendStop)
{
	Wire.beginTransmission(IMUAddress);
	Wire.write(registerAddress);
	Wire.write(data, length);
	uint8_t rcode = Wire.endTransmission(sendStop); // Returns 0 on success
	if (rcode) {
		Serial.print(F("i2cWrite failed: "));
		Serial.println(rcode);
	}
	return rcode; // See: http://arduino.cc/en/Reference/WireEndTransmission
}

uint8_t i2cRead(uint8_t registerAddress, uint8_t* data, uint8_t nbytes)
{
	uint32_t timeOutTimer;
	Wire.beginTransmission(IMUAddress);
	Wire.write(registerAddress);
	uint8_t rcode = Wire.endTransmission(false); // Don't release the bus
	if (rcode)
	{
		Serial.print(F("i2cRead failed: "));
		Serial.println(rcode);
		return rcode; // See: http://arduino.cc/en/Reference/WireEndTransmission
	}
	Wire.requestFrom(IMUAddress, nbytes, (uint8_t)true); // Send a repeated start and then release the bus after reading
	for (uint8_t i = 0; i < nbytes; i++)
	{
		if (Wire.available())
			data[i] = Wire.read();
		else
		{
			timeOutTimer = micros();
			while (((micros() - timeOutTimer) < I2C_TIMEOUT) && !Wire.available());
			if (Wire.available())
				data[i] = Wire.read();
			else
			{
				Serial.println(F("i2cRead timeout"));
				return 5; // This error value is not already taken by endTransmission
			}
		}
	}
	return 0; // Success
}

// I2C -------------------------------------------

// HPU 6050 --------------------------------------

Kalman kalmanX; // Create the Kalman instances
Kalman kalmanY;

/* IMU Data */
int16_t accX, accY, accZ;
int16_t gyroX, gyroY, gyroZ;

double accXangle, accYangle; // Angle calculate using the accelerometer
double gyroXangle, gyroYangle; // Angle calculate using the gyro
double kalAngleX, kalAngleY; // Calculate the angle using a Kalman filter

uint32_t timer;
uint8_t i2cData[14]; // Buffer for I2C data

void ReadHpu6050()
{
	while (i2cRead(0x3B, i2cData, 14));
	accX = ((i2cData[0] << 8) | i2cData[1]);
	accY = ((i2cData[2] << 8) | i2cData[3]);
	gyroX = ((i2cData[8] << 8) | i2cData[9]);
	gyroY = ((i2cData[10] << 8) | i2cData[11]);

	// atan2 outputs the value of -? to ? (radians) - see http://en.wikipedia.org/wiki/Atan2
	// We then convert it to 0 to 2? and then from radians to degrees
	accXangle = (atan2(accY, accZ) + PI) * RAD_TO_DEG;
	accYangle = (atan2(accX, accZ) + PI) * RAD_TO_DEG;

	double gyroXrate = (double)gyroX / 131.0;
	double gyroYrate = -((double)gyroY / 131.0);
	gyroXangle += gyroXrate * ((double)(micros() - timer) / 1000000); // Calculate gyro angle without any filter
	gyroYangle += gyroYrate * ((double)(micros() - timer) / 1000000);

	kalAngleX = kalmanX.getAngle(accXangle, gyroXrate, (double)(micros() - timer) / 1000000); // Calculate the angle using a Kalman filter
	kalAngleY = kalmanY.getAngle(accYangle, gyroYrate, (double)(micros() - timer) / 1000000);
	timer = micros();

	/* Print Data */
	Serial.print("\nGX: ");
	Serial.println(gyroX);

	Serial.print("GY: ");
	Serial.println(gyroY);

	Serial.print("AX: ");
	Serial.println(accX);
	
	Serial.print("AY: ");
	Serial.println(accY);
	
	Serial.print("KX:  ");
	Serial.println(kalAngleX);
	
	Serial.print("KY:  ");	
	Serial.println(kalAngleY);

	
}

// HPU 6050 --------------------------------------

// Stepper Motor ---------------------------------
// motor driver connections
// IN1, IN2, IN3, IN4
StepperMotor motor(2,3,4,5);
int motorSpeed = 8; // motor gets slower as this number increases

int angle = 90;
int stepsPerDegree = 4276 / 360 + 1;
int motorSteps = angle * stepsPerDegree;//  4320; //4100;//angle1 * 11;//4096; // 4076 is approx 360 degrees



int motorDelay = 500; // delay in ms before motor reverses

// Stepper Motor ---------------------------------

// Lidar -----------------------------------------

LIDARLite myLidarLite;

// Lidar -----------------------------------------


void setup()
{
	Serial.begin(115200);

	motor.setStepDuration(motorSpeed);

	Wire.begin();

	TWBR = ((F_CPU / 400000L) - 16) / 2; // Set I2C frequency to 400kHz

	i2cData[0] = 7; // Set the sample rate to 1000Hz - 8kHz/(7+1) = 1000Hz
	i2cData[1] = 0x00; // Disable FSYNC and set 260 Hz Acc filtering, 256 Hz Gyro filtering, 8 KHz sampling
	i2cData[2] = 0x00; // Set Gyro Full Scale Range to ±250deg/s
	i2cData[3] = 0x00; // Set Accelerometer Full Scale Range to ±2g
	while (i2cWrite(0x19, i2cData, 4, false)); // Write to all four registers at once
	while (i2cWrite(0x6B, 0x01, true)); // PLL with X axis gyroscope reference and disable sleep mode

	while (i2cRead(0x75, i2cData, 1));
	if (i2cData[0] != 0x68) { // Read "WHO_AM_I" register
		Serial.print(F("Error reading sensor"));
		while (1);
	}

	delay(100); // Wait for sensor to stabilize

	/* Set kalman and gyro starting angle */
	while (i2cRead(0x3B, i2cData, 6));
	accX = ((i2cData[0] << 8) | i2cData[1]);
	accY = ((i2cData[2] << 8) | i2cData[3]);
	accZ = ((i2cData[4] << 8) | i2cData[5]);
	// atan2 outputs the value of -? to ? (radians) - see http://en.wikipedia.org/wiki/Atan2
	// We then convert it to 0 to 2? and then from radians to degrees
	accYangle = (atan2(accX, accZ) + PI) * RAD_TO_DEG;
	accXangle = (atan2(accY, accZ) + PI) * RAD_TO_DEG;

	kalmanX.setAngle(accXangle); // Set starting angle
	kalmanY.setAngle(accYangle);
	gyroXangle = accXangle;
	gyroYangle = accYangle;

	timer = micros();

	Serial.println("Starting Lidar");
	myLidarLite.begin(0, true);
	myLidarLite.configure(0);

}

void loop()
{
	/* Update all the values */
	ReadHpu6050();
	delay(10);

	Serial.print("Distance:  ");	
	Serial.print(myLidarLite.distance());
	Serial.println("cm");	
	

	Serial.print("Steps:  ");	
	Serial.println(motorSteps);

	Serial.print("Angle:  ");	
	Serial.println(angle);

	// motor forwards
	motor.step(-motorSteps);

	//delay(motorDelay);

	// reverse motor
	motor.step(motorSteps);

	//delay(motorDelay);
}
