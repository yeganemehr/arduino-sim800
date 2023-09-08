#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ATConnection.hpp>
#include <Sim800.hpp>
#include <Sim800/Events.hpp>

SoftwareSerial swSerial(D3, D4); // RX & TX pins
ATConnection at(&swSerial);
Sim800 sim800(&at);

void setup()
{
    Serial.begin(9600);
    swSerial.begin(9600);

    sim800.setup();
    sim800.on(Sim800IncomingSMSEvent::type, [](const Event *e) {
        auto env = (Sim800IncomingSMSEvent *)e;
        Serial.printf("New SMS received (memory: %s, index: %hhu)\n", env->getMemory(), env->getIndex());

        Serial.println("Retrieve SMS from sim800");
        sim800.getSMS(env->getIndex())
            ->onSuccess([](const SMS *sms) {
                Serial.println("Retrieve SMS from sim800 [done]");
                if (sms->getType() == SMS::Type::TEXT_SMS) {
                    auto textSMS = (TextSMS *)sms;
                    Serial.println("Type: TEXT_SMS");
                    Serial.printf("Sender: %s\n", textSMS->getPeer());
                    Serial.printf("Text: \"%s\"\n", textSMS->getText());
                } else if (sms->getType() == SMS::Type::STATUS_SMS) {
                    Serial.println("Type: STATUS_SMS");
                } else {
                    Serial.println("Type: Unsupported");
                }
            })
            ->onFail([](const std::exception &reason) {
                Serial.println("Retrive SMS from sim800 [failed]");
            })
            ->freeOnFinish();
    });

    Serial.println("Initialize Sim800 connection");
    sim800.init()
        ->onSuccess([]() {
            Serial.println("Initialize Sim800 connection [done]");
            Serial.println("Put sim800 into sms text mode");
            sim800.putInTextMode()
                ->onSuccess([]() {
                    Serial.println("Put sim800 into sms text mode [done]");
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