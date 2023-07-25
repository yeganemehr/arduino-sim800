#include "Sim800.hpp"
#include <stdio.h>
#include <Arduino.h>
#include "Sim800/Events.hpp"

::Sim800::Sim800(ATConnection *at) : at(at)
{
}

void ::Sim800::setup()
{
	at->on(ATNotificationEvent::type, [this](const Event *e) {
		this->onAtNotification((const ATNotificationEvent *)e);
	});
}

void Sim800::onAtNotification(const ATNotificationEvent *e)
{
	if (e->content.indexOf("Call Ready") >= 0)
	{
		Sim800CallReadyEvent e;
		this->emit(&e);
		return;
	}
	if (e->content.indexOf("SMS Ready") >= 0)
	{
		Sim800SMSReadyEvent e;
		this->emit(&e);
		return;
	}
	if (e->content.indexOf("+CPIN: NOT READY") >= 0)
	{
		Sim800SimCardNotReadyEvent e;
		this->emit(&e);
		return;
	}
}

Promise<void> *Sim800::checkConnection() const
{
	return Promise<void>::from(at->execute("AT"), true);
}
Promise<void> *Sim800::disableEcho() const
{
	return Promise<void>::from(at->execute("ATE0"), true);
}
Promise<void> *Sim800::init() const
{
	return Promise<void>::sequence({
		std::bind(&Sim800::checkConnection, this),
		std::bind(&Sim800::disableEcho, this),
		std::bind(&Sim800::activeSMSNotification, this),
	});
}
Promise<uint32_t> *Sim800::sendAsciiSMS(const char *phone, const char *text) const {
	auto promise = new Promise<uint32_t>();
	char command[30];
	sprintf(command, "AT+CMGS=\"%s\"", phone);
	at->execute(command, text)
		->onSuccess([promise](const String &result) {
			uint32_t id;
			if (sscanf(result.c_str(), "+CMGS: %u", &id) == 1) {
				promise->resolve(id);
			} else {
				promise->reject(std::exception());
			}
		})
		->redirectRejectTo(promise)
		->freeOnFinish();
	return promise;
}

Promise<void> *Sim800::putInPDUMode()
{
	auto promise = new Promise<void>();
	at->setValue("CMGF", "0")
		->onSuccess([this, promise]() {
			this->smsMode = SMSMode::PDU_MODE;
			promise->resolve();
		})
		->redirectRejectTo(promise)
		->freeOnFinish();
	return promise;
}
Promise<void> *Sim800::putInTextMode()
{
	auto promise = new Promise<void>();
	at->setValue("CMGF", "1")
		->onSuccess([this, promise]() {
			this->smsMode = SMSMode::TEXT_MODE;
			promise->resolve();
		})
		->redirectRejectTo(promise)
		->freeOnFinish();
	return promise;
}
Promise<void> *Sim800::activeSMSNotification() const
{
	return at->setValue("CNMI", "2,1,0,0,0");
}
Promise<void> *Sim800::deactiveSMSNotification() const
{
	return at->setValue("CNMI", "2,0,0,0,0");
}
void Sim800::setCurrentSMSMode(Sim800::SMSMode smsMode)
{
	this->smsMode = smsMode;
}
Sim800::SMSMode Sim800::getCurrentSMSMode() const
{
	return smsMode;
}
Promise<void> *Sim800::deleteAllSMSes(DeleteSMSFilter filter) const
{
	if (smsMode == SMSMode::UNKNOWN) {
		auto promise = new Promise<void>();
		promise->reject(std::exception());
		return promise;
	}
	if (smsMode == SMSMode::TEXT_MODE) {
		const char *filters[] = {
			"\"DEL READ\"",
			"\"DEL UNREAD\"",
			"\"DEL SENT\"",
			"\"DEL UNSENT\"",
			"\"DEL INBOX\"",
			"\"DEL ALL\""};
		return at->setValue("CMGDA", filters[filter]);
	}
	char value[2];
	sprintf(value, "%hhu", filter);
	return at->setValue("CMGDA", value);
}
Promise<void> *Sim800::deleteSMS(uint8_t index) const
{
	char value[4];
	sprintf(value, "%hhu", index);
	return at->setValue("CMGD", value);
}
Promise<void> *Sim800::setBaudRate(uint32_t rate) const
{
	char srate[7];
	sprintf(srate, "%u", rate);
	return at->setValue("IPR", srate);
}

Promise<std::vector<SMS *>> *Sim800::getSMSList(Sim800::SMSListFilter filter, bool changeStatus) const
{
	auto promise = new Promise<std::vector<SMS *>>();
	if (smsMode == SMSMode::UNKNOWN)
	{
		promise->reject(std::exception());
		return promise;
	}
	char value[23] = "AT+CMGL=";
	if (smsMode == SMSMode::TEXT_MODE)
	{
		switch (filter)
		{
		case SMSListFilter::INBOX_UNREAD:
			strcat(value, "\"REC UNREAD\"");
			break;
		case SMSListFilter::INBOX_READ:
			strcat(value, "\"REC READ\"");
			break;
		case SMSListFilter::STORED_SENT:
			strcat(value, "\"STO SENT\"");
			break;
		case SMSListFilter::STORED_UNSENT:
			strcat(value, "\"STO UNSENT\"");
			break;
		case SMSListFilter::SMS_ALL:
			strcat(value, "\"ALL\"");
			break;
		}
	}
	else
	{
		sprintf(value + 8, "%hhu", filter);
	}
	strcat(value, changeStatus ? ",1" : ",0");
	at->execute(value)
		->onSuccess([promise](const String &result) {
			promise->resolve(SMS::parseCMGL(result));
		})
		->redirectRejectTo(promise)
		->freeOnFinish();
	return promise;
}

Promise<int8_t> *Sim800::getSignalLevel() const
{
	auto promise = new Promise<int8_t>();
	at->execute("AT+CSQ")
		->onSuccess([promise](const String &result) {
			uint8_t rssi;
			if (sscanf(result.c_str(), "+CSQ: %hhu,", &rssi) == 1)
			{
				int8_t result;
				if (rssi <= 31)
				{
					result = map(rssi, 0, 31, -115, -52);
				} else {
					result = 99;
				}
				promise->resolve(result);
			}
			else
			{
				promise->reject(std::exception());
			}
		})
		->redirectRejectTo(promise)
		->freeOnFinish();
	return promise;
}

Promise<String> *Sim800::getNetworkName() const
{
	auto promise = new Promise<String>();
	at->execute("AT+COPS?")
		->onSuccess([promise](const String &result) {
			uint8_t mode;
			uint8_t format;
			char operatorName[64];
			if (sscanf(result.c_str(), "+COPS: %hhu,%hhu,\"%s\"", &mode, &format, operatorName) >= 0)
			{
				auto len = strlen(operatorName);
				if (operatorName[len - 1] == '"') {
					operatorName[len - 1] = 0;
				}
				promise->resolve(operatorName);
			}
			else
			{
				promise->reject(std::exception());
			}
		})
		->redirectRejectTo(promise)
		->freeOnFinish();
	return promise;
}
