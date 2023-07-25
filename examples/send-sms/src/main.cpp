#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ATConnection.hpp>
#include <Sim800.hpp>

SoftwareSerial swSerial(D5, D0);
ATConnection at(&swSerial);
Sim800 sim800(&at);

void setup()
{
	Serial.begin(9600);
	swSerial.begin(9600);
	Serial.println("Initialize Sim800 connection");
	sim800.init()
		->onSuccess([]() {
			Serial.println("Initialize Sim800 connection [done]");
			Serial.println("Put sim800 into sms text mode");
			sim800.putInTextMode()
				->onSuccess([]() {
					Serial.println("Put sim800 into sms text mode [done]");

					Serial.println("Sending text");
					sim800.sendAsciiSMS("+100000000", "Hi")
						->onSuccess([](const uint32_t &id) {
							Serial.printf("Sending text [ID: %u]\n", id);
						})
						->onFail([](const std::exception &reason) {
							Serial.println("Sending text [failed]");
						})
						->freeOnFinish();
				})
				->onFail([](const std::exception &reason) {
					Serial.println("Put sim800 into sms text mode [failed]");
				})
				->freeOnFinish();

		})
		->onFail([](const std::exception &reason) {
			Serial.println("Initialize Sim800 connection [failed]");
		})
		->freeOnFinish();
}

void loop()
{
	at.communicate();
}