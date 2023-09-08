#include "SMS.hpp"

size_t consumeSMSHeaderPart(const char *str, size_t endHeader, size_t &start, size_t &end);
SMS::Status parseStatus(const char *status);

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
		SMS *sms = SMS::parse("+CMGL:", CMGL, fromIndex, error);
		if (sms != nullptr)
		{
			list.push_back(sms);
		}
	} while (fromIndex < len && lastFromIndex != fromIndex);
	return list;
}

SMS *SMS::parse(const String &header, const String &content, size_t &fromIndex, uint8_t &error)
{
	bool isCMGL = header == "+CMGL:";

	auto headerStart = content.indexOf(header, fromIndex);
	if (headerStart == -1)
	{
		error = 1;
		return nullptr;
	}
	auto headerEnd = content.indexOf('\r', headerStart + 1);
	if (headerEnd == -1)
	{
		error = 2;
		return nullptr;
	}
	auto endPos = content.indexOf("\r\n" + header, headerEnd + 2);
	if (endPos == -1)
	{
		endPos = content.length();
	}

	fromIndex = headerStart + header.length() + 1;
	bool isText = endPos - headerEnd > 2;

	if (!isText) {
		
		return nullptr;
	}

	SMS::Status status = SMS::Status::INBOX_UNREAD;
	uint8_t id = 0;
	String peer;

	for (uint8_t x = 0; x < 3; x++) {
		size_t start = fromIndex;
		size_t length = consumeSMSHeaderPart(content.c_str(), headerEnd, start, fromIndex);
		if (start == fromIndex) {
			error = 3;
			return nullptr;
		}
		auto part = content.substring(start, start + length);
		if (isCMGL) {
			switch (x) {
				case 0: id = part.toInt(); break;
				case 1: status = parseStatus(part.c_str()); break;
				case 2: peer = std::move(part); break;
			}
		} else {
			switch (x) {
				case 0: status = parseStatus(part.c_str()); break;
				case 1: peer = std::move(part); break;
			}
		}
	}
	fromIndex = endPos;
	return new TextSMS(id, status, peer.c_str(), content.substring(headerEnd + 2, endPos).c_str());
}

SMS *SMS::parseCMGR(const String &CMGR)
{
	size_t fromIndex = 0;
	uint8_t error = 0;
	return SMS::parse("+CMGR:", CMGR, fromIndex, error);
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

size_t consumeSMSHeaderPart(const char *str, size_t endHeader, size_t &start, size_t &end)
{
	if (start >= endHeader) {
		return 0;
	}
	bool inValue = false;
	size_t length = 0;
	for (end = start; end < endHeader; end++) {
		char ch = str[end];
		if (!inValue) {
			if (isspace(ch)) {
				start++;
			} else if (ch == '"') {
				inValue = true;
				start++;
			} else if (ch == ',') {
				end++;
				break;
			} else {
				length++;
			}
		} else {
			if (ch == '"') {
				inValue = false;
			} else {
				length++;
			}
		}
	}
	return length;
}

SMS::Status parseStatus(const char *status)
{
	const char *statuses[] = {"REC UNREAD", "REC READ", "STO SENT", "STO UNSENT"};
	for (uint8_t x = 0; x < 4; x++)
	{
		if (strcmp(status, statuses[x]) == 0)
		{
			return (SMS::Status)x;
		}
	}

	return SMS::Status::INBOX_UNREAD;
}