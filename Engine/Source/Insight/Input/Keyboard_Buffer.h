#pragma once

#include "Insight/Core.h"

#include "InputBuffer.h"

namespace Insight {

	class INSIGHT_API KeyboardBuffer : public InputBuffer
	{
	public:
		KeyboardBuffer();
		~KeyboardBuffer();
		
		KeyPressState GetKeyState(const unsigned char key);
		void OnKeyPressed(const unsigned char key);
		void OnKeyReleased(const unsigned char key);
	private:
		bool keyStates[256] = { false };
	};
	
}
