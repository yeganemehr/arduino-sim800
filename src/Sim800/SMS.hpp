#ifndef SIM800_SMS_HPP
#define SIM800_SMS_HPP

#include <WString.h>
#include <vector>

class SMS
{
public:
	enum Type : uint8_t
	{
		TEXT_SMS,
		STATUS_SMS,
	};
	enum Status
	{
		INBOX_UNREAD = 0,
		INBOX_READ = 1,
		STORED_UNSENT = 2,
		STORED_SENT = 3,
	};

protected:
	uint8_t id;
	Status status;

public:
	static std::vector<SMS *> parseCMGL(const String &CMGL);
	static SMS *parse(const String &header, const String &content, size_t &fromIndex, uint8_t &error);
	static SMS *parseCMGR(const String &CMGR);

	SMS(uint8_t id, Status status) : id(id), status(status){};
	virtual ~SMS()
	{
	}
	virtual Type getType() const = 0;
	inline Status getStatus() const
	{
		return status;
	}

	inline void setStatus(Status status)
	{
		this->status = status;
	}

	inline uint8_t getId() const {
		return id;
	}

	inline void setId(uint8_t id)
	{
		this->id = id;
	}
};

class TextSMS : public SMS
{

protected:
	char *peer;
	char *text;

public:
	TextSMS(const TextSMS &) = delete;
	TextSMS(TextSMS &&);
	TextSMS(uint8_t id, Status status) : SMS(id, status) {}
	TextSMS(uint8_t id, Status status, const char *peer, const char *text);
	~TextSMS() {
		delete[] peer;
		delete[] text;
	}
	virtual Type getType() const
	{
		return Type::TEXT_SMS;
	}
	inline char *getPeer() const
	{
		return peer;
	}
	inline char *getText() const
	{
		return text;
	}
	void setText(const char *text);
	void setPeer(const char *peer);
	TextSMS &operator=(const TextSMS &) = delete;
	TextSMS &operator=(TextSMS &&);
	inline void loseOwnership() {
		peer = nullptr;
		text = nullptr;
	}
};

#endif
