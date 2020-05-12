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

}

