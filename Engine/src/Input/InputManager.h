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
	void SetMouseY(float y) { m_mouseY = y; }
	void SetMouseX(float x) { m_mouseX = x; }

	float GetMouseX() const { return m_mouseX; }
	float GetMouseY() const { return m_mouseY; }
	void MouseMoved(bool moved) { m_mouseMoved = moved; }
	bool GetMouseMoved() const { return m_mouseMoved; }

private:
	bool m_mouseMoved = false;
	float m_mouseX = 0.0f;
	float m_mouseY = 0.0f;
};