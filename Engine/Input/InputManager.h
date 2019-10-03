#pragma once
#include "Keyboard\\KeyboardClass.h"
#include "Mouse\\MouseClass.h"
#include "..\\Framework\\Singleton.h"

class InputManager : public Singleton<InputManager>
{
public:
	InputManager() {}
	~InputManager() {}

private:
	KeyboardClass m_keyboard;
	MouseClass m_mouse;
};