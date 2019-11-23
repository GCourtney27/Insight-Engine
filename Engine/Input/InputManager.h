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
	void  SetMouseX(float x) { m_mouseX = x; }
	void  SetMouseY(float y) { m_mouseY = y; }

	float GetMouseX() { return m_mouseX; }
	float GetMouseY() { return m_mouseY; }
	void MouseMoved(bool moved) { m_mouseMoved = moved; }
	bool GetMouseMoved() { return m_mouseMoved; }

private:
	bool m_mouseMoved = false;
	float m_mouseX = 0;
	float m_mouseY = 0;
};