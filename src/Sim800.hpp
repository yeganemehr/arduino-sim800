#ifndef SIM800_HPP
#define SIM800_HPP

#include <ATConnection.hpp>
#include "Sim800/SMS.hpp"

class Sim800 : public EventEmitter
{
public:
	enum DeleteSMSFilter : uint8_t {
		DELETE_READ,
		DELETE_UNREAD,
		DELETE_SENT,
		DELETE_UNSENT,
		DELETE_INBOX,
		DELETE_ALL,
	};
	enum SMSListFilter : uint8_t
	{
		INBOX_UNREAD,
		INBOX_READ,
		STORED_UNSENT,
		STORED_SENT,
		SMS_ALL,
	};
	enum SMSMode : uint8_t
	{
		UNKNOWN,
		TEXT_MODE,
		PDU_MODE,
	};
	Sim800(ATConnection *at);
	void setup();
	Promise<void> *checkConnection() const;
	Promise<void> *disableEcho() const;
	Promise<void> *detachGprs() const;
	Promise<void> *initApn() const;
	Promise<void> *startGprs() const;
	Promise<void> *init() const;
	Promise<void> *initGprs() const;
	Promise<void> *tcpConnect(const char* host, uint16_t port) const;
	Promise<void> *tcpClose();
	Promise<void> *putInPDUMode();
	Promise<void> *putInTextMode();
	Promise<void> *sendBinaryData(const byte *data, size_t length) const;
	Promise<uint32_t> *sendAsciiSMS(const char *phone, const char *text) const;
	Promise<void> *activeSMSNotification() const;
	Promise<void> *deactiveSMSNotification() const;
	Promise<void> *deleteAllSMSes(DeleteSMSFilter filter = DeleteSMSFilter::DELETE_ALL) const;
	Promise<void> *deleteSMS(uint8_t index) const;
	void setCurrentSMSMode(SMSMode smsMode);
	Promise<String> *getLocalIp() const;
	Promise<String> *getIMEI() const;
	SMSMode getCurrentSMSMode() const;
	Promise<void> *setBaudRate(uint32_t rate) const;
	Promise<std::vector<SMS *>> *getSMSList(SMSListFilter filter = SMSListFilter::INBOX_UNREAD, bool changeStatus = true) const;
	Promise<int8_t> *getSignalLevel() const;
	Promise<String> *getNetworkName() const;
	Promise<String> *getICCID() const;
	Promise<SMS *> *getSMS(uint8_t index, bool changeStatus = true) const;
	void setApn(const char *apn, const char *user, const char *pass);
private:
	ATConnection *at;
	SMSMode smsMode = SMSMode::UNKNOWN;
	void onAtNotification(const ATNotificationEvent *e);
	char stapn[20] = {'\0'};
	char stuser[20] = {'\0'};
	char stpass[20] = {'\0'};
};

#endif
