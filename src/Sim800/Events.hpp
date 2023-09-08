#ifndef SIM800_EVENTS_HPP
#define SIM800_EVENTS_HPP

#include <Event.hpp>
#include <WString.h>

class Sim800CallReadyEvent : public Event
{
public:
	constexpr static event_type_t type = 0xff021001;
	event_type_t getType() const
	{
		return type;
	}
};

class Sim800SMSReadyEvent : public Event
{
public:
	constexpr static event_type_t type = 0xff021002;
	event_type_t getType() const
	{
		return type;
	}
};

class Sim800SimCardNotReadyEvent : public Event
{
public:
	constexpr static event_type_t type = 0xff021003;
	event_type_t getType() const
	{
		return type;
	}
};

class Sim800IncomingSMSEvent : public Event
{
private:
	char memory[5] = {'\0'};
	uint8_t index = 0;
public:
	constexpr static event_type_t type = 0xff021004;

	static bool parseCMTI(Sim800IncomingSMSEvent *dest, const String &CMTI);
	Sim800IncomingSMSEvent() {};
	Sim800IncomingSMSEvent(const char *memory, uint8_t index);

	event_type_t getType() const
	{
		return type;
	}

	inline const char *getMemory() const
	{
		return memory;
	}

	inline uint8_t getIndex() const
	{
		return index;
	}

};

#endif
