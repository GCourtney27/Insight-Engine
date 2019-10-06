#pragma once
#include "Keyboard\\KeyboardClass.h"
#include "Mouse\\MouseClass.h"
#include "..\\Framework\\Singleton.h"

class InputManager : public Singleton<InputManager>
{
public:
	InputManager() {}
	~InputManager() {}

	KeyboardClass keyboard;
	MouseClass mouse;
private:
};