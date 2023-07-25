#ifndef SIM800_EVENTS_HPP
#define SIM800_EVENTS_HPP

#include <Event.hpp>

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

#endif
