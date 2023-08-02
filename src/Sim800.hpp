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
	Promise<void> *init() const;
	Promise<void> *putInPDUMode();
	Promise<void> *putInTextMode();
	Promise<uint32_t> *sendAsciiSMS(const char *phone, const char *text) const;
	Promise<void> *activeSMSNotification() const;
	Promise<void> *deactiveSMSNotification() const;
	Promise<void> *deleteAllSMSes(DeleteSMSFilter filter = DeleteSMSFilter::DELETE_ALL) const;
	Promise<void> *deleteSMS(uint8_t index) const;
	void setCurrentSMSMode(SMSMode smsMode);
	SMSMode getCurrentSMSMode() const;
	Promise<void> *setBaudRate(uint32_t rate) const;
	Promise<std::vector<SMS *>> *getSMSList(SMSListFilter filter = SMSListFilter::INBOX_UNREAD, bool changeStatus = true) const;
	Promise<int8_t> *getSignalLevel() const;
	Promise<String> *getNetworkName() const;
	Promise<String> *getICCID() const;
private:
	ATConnection *at;
	SMSMode smsMode = SMSMode::UNKNOWN;
	void onAtNotification(const ATNotificationEvent *e);
};

#endif