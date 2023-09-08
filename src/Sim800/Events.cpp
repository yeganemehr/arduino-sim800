#include "Events.hpp"
#include <string.h>
#include <cstdio>

Sim800IncomingSMSEvent::Sim800IncomingSMSEvent(const char *memory, uint8_t index): index(index)
{
	strlcpy(this->memory, memory, sizeof(this->memory));
}

bool Sim800IncomingSMSEvent::parseCMTI(Sim800IncomingSMSEvent *dest, const String &CMTI)
{
	int memPos = CMTI.indexOf("+CMTI:");
	if (memPos < 0) {
		return false;
	}
	return sscanf(CMTI.c_str() + memPos, "+CMTI: \"%[^\"]\",%hhu", dest->memory, &dest->index) == 2;
}