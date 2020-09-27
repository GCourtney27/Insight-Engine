#pragma once

#include <Retina/Core.h>

#include "Retina/Input/Input_Buffer.h"

namespace Retina {

	class RETINA_API KeyboardBuffer : public InputBuffer
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
