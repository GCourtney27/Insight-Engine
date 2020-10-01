#include <Engine_pch.h>

#include "Keyboard_Buffer.h"

namespace Insight {

	
	
	KeyboardBuffer::KeyboardBuffer()
	{
	}

	KeyboardBuffer::~KeyboardBuffer()
	{
	}

	KeyboardBuffer::KeyPressState KeyboardBuffer::GetKeyState(const unsigned char key)
	{
		if (keyStates[key] == true)
			return KeyPressState::PRESSED;
		else
			return KeyPressState::RELEASED;
	}

	void KeyboardBuffer::OnKeyPressed(const unsigned char key)
	{
		this->keyStates[key] = true;
	}

	void KeyboardBuffer::OnKeyReleased(const unsigned char key)
	{
		this->keyStates[key] = false;
	}

}