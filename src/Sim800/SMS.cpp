#include "SMS.hpp"

std::vector<SMS *> SMS::parseCMGL(const String &CMGL)
{
	size_t len = CMGL.length();
	size_t lastFromIndex = 0;
	size_t fromIndex = 0;
	std::vector<SMS *> list;
	do
	{
		lastFromIndex = fromIndex;
		uint8_t error = 0;
		SMS *sms = SMS::parseCMGL(CMGL, fromIndex, error);
		if (sms != nullptr)
		{
			list.push_back(sms);
		}
	} while (fromIndex < len && lastFromIndex != fromIndex);
	return list;
}

SMS *SMS::parseCMGL(const String &CMGL, size_t &fromIndex, uint8_t &error)
{
	auto headerStart = CMGL.indexOf("+CMGL:", fromIndex);
	if (headerStart == -1)
	{
		error = 1;
		return nullptr;
	}
	auto headerEnd = CMGL.indexOf('\r', headerStart + 1);
	if (headerEnd == -1)
	{
		error = 2;
		return nullptr;
	}
	auto endPos = CMGL.indexOf("\r\n+CMGL:", headerEnd + 2);
	if (endPos == -1)
	{
		endPos = CMGL.length();
	}
	fromIndex = endPos;
	int comma[4];
	size_t lastPos = headerStart;
	for (uint8_t x = 0; x < 4; x++)
	{
		comma[x] = CMGL.indexOf(',', lastPos);
		if (comma[x] < 0)
		{
			error = 4;
			return nullptr;
		}
		lastPos = comma[x] + 1;
	}
	uint8_t id = CMGL.substring(headerStart + 7, comma[0]).toInt();

	SMS::Status status = SMS::Status::INBOX_UNREAD;
	const char *statuses[] = {
		"\"REC UNREAD\"",
		"\"REC READ\"",
		"\"STO SENT\"",
		"\"STO UNSENT\""};

	for (uint8_t x = 0; x < 4; x++)
	{
		
		if (memcmp(CMGL.c_str() + comma[0] + 1, statuses[x], strlen(statuses[x]) - 1) == 0)
		{
			status = (SMS::Status)x;
			break;
		}
	}

	if (endPos - headerEnd > 2)
	{
		String peer = CMGL.substring(comma[1] + 1, comma[2]);
		if (peer.charAt(0) == '"')
		{
			peer.remove(0, 1);
		}
		const size_t lastIndex = peer.length() - 1;
		if (peer.charAt(lastIndex) == '"')
		{
			peer.remove(lastIndex, 1);
		}
		return new TextSMS(id, status, peer.c_str(), CMGL.substring(headerEnd + 2, endPos).c_str());
	}
	return nullptr;
}

TextSMS::TextSMS(TextSMS &&other): SMS(other.id, other.status)
{
	peer = other.peer;
	text = other.text;
	other.peer = nullptr;
	other.text = nullptr;
}
TextSMS::TextSMS(uint8_t id, Status status, const char *peer, const char *text) : SMS(id, status)
{
	size_t len;

	len = strlen(peer) + 1;
	this->peer = new char[len];
	memcpy(this->peer, peer, len);

	len = strlen(text) + 1;
	this->text = new char[len];
	memcpy(this->text, text, len);
}
void TextSMS::setText(const char *text)
{
	delete[] this->text;
	if (text == nullptr)
	{
		this->text = nullptr;
		return;
	}
	size_t len = strlen(text) + 1;
	this->text = new char[len];
	memcpy(this->text, text, len);
}
void TextSMS::setPeer(const char *peer)
{
	delete[] this->peer;
	if (peer == nullptr)
	{
		this->peer = nullptr;
		return;
	}
	size_t len = strlen(peer) + 1;
	this->peer = new char[len];
	memcpy(this->peer, peer, len);
}
TextSMS &TextSMS::operator = (TextSMS &&other) {
	if (&other == this) {
		return *this;
	}
	delete[] peer;
	delete[] text;
	id = other.id;
	status = other.status;
	peer = other.peer;
	text = other.text;
	other.peer = nullptr;
	other.text = nullptr;

	return *this;
}
