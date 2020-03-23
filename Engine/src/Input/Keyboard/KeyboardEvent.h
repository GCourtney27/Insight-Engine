#pragma once

class KeyboardEvent
{
public:
	enum EventType
	{
		Press,
		Release,
		Invalid,
	};

	KeyboardEvent();
	KeyboardEvent(const EventType type, const unsigned char key);
	bool IsPress() const;
	bool IsRealesed() const;
	bool IsValid() const;
	unsigned char GetKeyCode() const;

private:
	EventType type;
	unsigned char key;
};