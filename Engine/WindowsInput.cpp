#include "ie_pch.h"
#include "WindowsInput.h"

#include "Insight/Application.h"
#include "Platform/Windows/Windows_Window.h"


namespace Insight {

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<WindowsWindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = KeyboardBuffer::Get().GetKeyState(keycode);
		return state == InputBuffer::PRESSED;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<WindowsWindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = MouseBuffer::Get().GetButtonState(button);
		return state == InputBuffer::PRESSED;
	}

	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		auto window = static_cast<WindowsWindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xPos, yPos;
		MouseBuffer::Get().GetMousePosition(xPos, yPos);
		//glfwGetCursorPos(window, &xPos, &yPos);
		return { (float)xPos, (float)yPos };
		//return {0, 0};
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