#pragma once

#include <Retina/Core.h>

#include "Retina/Events/Key_Event.h"
#include "Retina/Events/Mouse_Event.h"

#include "Retina/Input/Keyboard_Buffer.h"
#include "Retina/Input/Mouse_Buffer.h"

namespace Retina {


	class RETINA_API InputManager
	{
	public:
		InputManager() {}
		~InputManager() {}

		void OnEvent(Event& event);

		KeyboardBuffer& GetKeyboardBuffer() { return m_KeyboardBuffer; }
		MouseBuffer& GetMouseBuffer() { return m_MouseBuffer; }

	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseScrollEvent(MouseScrolledEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnRawMouseMoveEvent(MouseRawMoveEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
	private:
		MouseBuffer m_MouseBuffer;
		KeyboardBuffer m_KeyboardBuffer;
	};

}
