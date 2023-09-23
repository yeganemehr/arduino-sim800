#ifndef SIM800_EVENTS_HPP
#define SIM800_EVENTS_HPP

#include <Event.hpp>
#include <WString.h>
#include <time.h>

enum {CONNECTED, FAILED, DISCONNECTED};

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

class Sim800IncomingGSMEvent : public Event
{
private:
	uint8_t index = 0;
	uint16_t ident = 0;
	char network[20] = {'\0'};
public:
	constexpr static event_type_t type = 0xff021005;

	static bool parseCIEV(Sim800IncomingGSMEvent *status, const String &CIEV);
	Sim800IncomingGSMEvent() {};
	Sim800IncomingGSMEvent(uint16_t ident, const char *network);

	event_type_t getType() const
	{
		return type;
	}

	inline const uint16_t getIdent() const
	{
		return ident;
	}

	inline const char *getNetwork() const
	{
		return network;
	}

};

class Sim800TCPEvent : public Event
{
public:
	enum TCPState : uint8_t
	{
		CLOSED,
		CONNECTED,
		FAILED,
	};
	TCPState status = CLOSED;
	constexpr static event_type_t type = 0xff021006;

	event_type_t getType() const
	{
		return type;
	}

	inline TCPState getState() const
	{
		return status;
	}
};

class Sim800IncomingDateEvent : public Event
{
public:
	time_t gmt = 0;
	int dsec = 0;
	bool dst = false;
	constexpr static event_type_t type = 0xff021007;

	static bool parsePSUTTZ(Sim800IncomingDateEvent *date, const String &PSUTTZ);

	event_type_t getType() const
	{
		return type;
	}

	inline time_t getTime()
	{
		return gmt;
	}

	inline int getDsec()
	{
		return dsec;
	}

	inline bool getDst()
	{
		return dst;
	}
};

#endif
