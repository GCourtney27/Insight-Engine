#include <ie_pch.h>

#include "Mouse_Buffer.h"

namespace Insight {


	MouseBuffer::MouseBuffer()
	{
	}

	MouseBuffer::~MouseBuffer()
	{
	}

	InputBuffer::KeyPressState MouseBuffer::GetButtonState(const BYTE button)
	{
		if (m_Buttons[button] == true)
			return KeyPressState::PRESSED;
		else
			return KeyPressState::RELEASED;
	}

	void MouseBuffer::OnButtonPressed(const BYTE button)
	{
		m_Buttons[button] = true;
	}

	void MouseBuffer::OnButtonReleased(const BYTE button)
	{
		m_Buttons[button] = false;
	}

	void MouseBuffer::OnMouseScroll(float xOffset, float yOffset)
	{	
		// Scrolling Up
		if (yOffset > 0.0f)
			m_ScrollState[0] = true;
		else
			m_ScrollState[0] = false;

		// Scrolling Down
		if (yOffset < 0.0f)
			m_ScrollState[1] = true;
		else
			m_ScrollState[1] = false;

		// Scorll Left
		if (xOffset > 0.0f)
			m_ScrollState[2] = true;
		else
			m_ScrollState[2] = false;

		// Scorll Right
		if (xOffset > 0.0f)
			m_ScrollState[3] = true;
		else
			m_ScrollState[3] = false;

		m_MouseScrollXOffset = xOffset;
		m_MouseScrollYOffset = yOffset;
	}

	InputBuffer::MouseScrollState MouseBuffer::GetScrollWheelState()
	{
		if (m_ScrollState[0] == true)
			return InputBuffer::SCROLL_UP;
		else if (m_ScrollState[0] == false)
			return InputBuffer::SCROLL_DOWN;
	}

}

