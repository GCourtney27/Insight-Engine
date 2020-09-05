#include <ie_pch.h>

#include "Input_Manager.h"

#include "Insight/Events/Key_Event.h"
#include "Insight/Events/Mouse_Event.h"

namespace Insight {



	void InputManager::OnEvent(Event& event)
	{
		EventDispatcher Dispatcher(event);
		// Mouse Buttons
		Dispatcher.Dispatch<MouseButtonPressedEvent>(IE_BIND_EVENT_FN(InputManager::OnMouseButtonPressedEvent));
		Dispatcher.Dispatch<MouseButtonReleasedEvent>(IE_BIND_EVENT_FN(InputManager::OnMouseButtonReleasedEvent));
		// Mouse Moved
		Dispatcher.Dispatch<MouseMovedEvent>(IE_BIND_EVENT_FN(InputManager::OnMouseMovedEvent));
		Dispatcher.Dispatch<MouseRawMoveEvent>(IE_BIND_EVENT_FN(InputManager::OnRawMouseMoveEvent));
		// Mouse Scroll
		Dispatcher.Dispatch<MouseScrolledEvent>(IE_BIND_EVENT_FN(InputManager::OnMouseScrollEvent));
		// Key Pressed
		Dispatcher.Dispatch<KeyPressedEvent>(IE_BIND_EVENT_FN(InputManager::OnKeyPressedEvent));
		Dispatcher.Dispatch<KeyReleasedEvent>(IE_BIND_EVENT_FN(InputManager::OnKeyReleasedEvent));
		// Key Typed
		Dispatcher.Dispatch<KeyTypedEvent>(IE_BIND_EVENT_FN(InputManager::OnKeyTypedEvent));
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