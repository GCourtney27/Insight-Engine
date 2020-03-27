#include "ie_pch.h"
#include "Windows_Input.h"

#include "Insight/Application.h"
#include "Platform/Windows/Windows_Window.h"


namespace Insight {

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto state = KeyboardBuffer::Get().GetKeyState(keycode);
		return state == InputBuffer::PRESSED;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto state = MouseBuffer::Get().GetButtonState(button);
		return state == InputBuffer::PRESSED;
	}

	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		double xPos, yPos;
		MouseBuffer::Get().GetMousePosition(xPos, yPos);
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