#include "ie_pch.h"
#include "Windows_Input.h"

#include "Insight/Core/Application.h"
#include "Platform/Windows/Windows_Window.h"


namespace Insight {

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto state = m_InputManager.GetKeyboardBuffer().GetKeyState(keycode);
		return state == InputBuffer::PRESSED;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto state = m_InputManager.GetMouseBuffer().GetButtonState(button);
		return state == InputBuffer::PRESSED;
	}

	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		double xPos, yPos;
		m_InputManager.GetMouseBuffer().GetMousePosition(xPos, yPos);
		return { (float)xPos, (float)yPos };
	}

	std::pair<float, float> WindowsInput::GetMouseRawPositionImpl()
	{
		double xPos, yPos;
		m_InputManager.GetMouseBuffer().GetRawMousePosition(xPos, yPos);
		return { (float)xPos, (float)yPos };
	}

	float WindowsInput::GetMouseXImpl()
	{
		auto[x, y] = GetMousePositionImpl();
		return (float)x;
	}

	float WindowsInput::GetMouseYImpl()
	{
		auto[x, y] = GetMousePositionImpl();
		return (float)y;
	}

}