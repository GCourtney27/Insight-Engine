#pragma once

#include "Event.h"

class EventReciever
{
public:
	EventReciever() {}

	virtual void OnEvent(const Event& event) = 0;
};