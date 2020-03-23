#pragma once
#include "../Objects/ID.h"

class Entity;

struct Event
{
	Event() {}

	ID eventID;
	Entity* sender;
	Entity* reviever;
	//Varient varient;

};
