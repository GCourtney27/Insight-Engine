#include <Engine_pch.h>

#include "Input_Manager.h"

#include "Retina/Events/Key_Event.h"
#include "Retina/Events/Mouse_Event.h"

namespace Retina {



	void InputManager::OnEvent(Event& event)
	{
		EventDispatcher Dispatcher(event);
		// Mouse Buttons
		Dispatcher.Dispatch<MouseButtonPressedEvent>(RN_BIND_EVENT_FN(InputManager::OnMouseButtonPressedEvent));
		Dispatcher.Dispatch<MouseButtonReleasedEvent>(RN_BIND_EVENT_FN(InputManager::OnMouseButtonReleasedEvent));
		// Mouse Moved
		Dispatcher.Dispatch<MouseMovedEvent>(RN_BIND_EVENT_FN(InputManager::OnMouseMovedEvent));
		Dispatcher.Dispatch<MouseRawMoveEvent>(RN_BIND_EVENT_FN(InputManager::OnRawMouseMoveEvent));
		// Mouse Scroll
		Dispatcher.Dispatch<MouseScrolledEvent>(RN_BIND_EVENT_FN(InputManager::OnMouseScrollEvent));
		// Key Pressed
		Dispatcher.Dispatch<KeyPressedEvent>(RN_BIND_EVENT_FN(InputManager::OnKeyPressedEvent));
		Dispatcher.Dispatch<KeyReleasedEvent>(RN_BIND_EVENT_FN(InputManager::OnKeyReleasedEvent));
		// Key Typed
		Dispatcher.Dispatch<KeyTypedEvent>(RN_BIND_EVENT_FN(InputManager::OnKeyTypedEvent));
	}

	bool InputManager::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e)
	{
		m_MouseBuffer.OnButtonPressed(e.GetMouseButton());
		return false;
	}

	bool InputManager::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
	{
		m_MouseBuffer.OnButtonReleased(e.GetMouseButton());
		return false;
	}

	bool InputManager::OnMouseScrollEvent(MouseScrolledEvent& e)
	{
		m_MouseBuffer.OnMouseScroll(e.GetXOffset(), e.GetYOffset());
		return false;
	}

	bool InputManager::OnMouseMovedEvent(MouseMovedEvent& e)
	{
		m_MouseBuffer.OnMousePositionMoved(e.GetX(), e.GetY());
		return false;
	}

	bool InputManager::OnRawMouseMoveEvent(MouseRawMoveEvent& e)
	{
		m_MouseBuffer.OnRawMousePositionMoved(e.GetX(), e.GetY());
		return false;
	}

	bool InputManager::OnKeyReleasedEvent(KeyReleasedEvent& e)
	{
		m_KeyboardBuffer.OnKeyReleased(e.GetKeyCode());
		return false;
	}

	bool InputManager::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		m_KeyboardBuffer.OnKeyPressed(e.GetKeyCode());
		return false;
	}

	bool InputManager::OnKeyTypedEvent(KeyTypedEvent& e)
	{
		return false;
	}

}