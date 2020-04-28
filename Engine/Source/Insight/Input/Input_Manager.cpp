#include "ie_pch.h"

#include "Input_Manager.h"

#include "Insight/Events/Key_Event.h"
#include "Insight/Events/Mouse_Event.h"

namespace Insight {

	void InputManager::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		// Mouse Buttons
		dispatcher.Dispatch<MouseButtonPressedEvent>(IE_BIND_EVENT_FN(InputManager::OnMouseButtonPressedEvent));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(IE_BIND_EVENT_FN(InputManager::OnMouseButtonReleasedEvent));
		// Mouse Moved
		dispatcher.Dispatch<MouseMovedEvent>(IE_BIND_EVENT_FN(InputManager::OnMouseMovedEvent));
		dispatcher.Dispatch<MouseRawMoveEvent>(IE_BIND_EVENT_FN(InputManager::OnRawMouseMoveEvent));
		dispatcher.Dispatch<MouseScrolledEvent>(IE_BIND_EVENT_FN(InputManager::OnMouseScrollEvent));
		// Key Pressed
		dispatcher.Dispatch<KeyPressedEvent>(IE_BIND_EVENT_FN(InputManager::OnKeyPressedEvent));
		dispatcher.Dispatch<KeyReleasedEvent>(IE_BIND_EVENT_FN(InputManager::OnKeyReleasedEvent));
		// Key Typed
		dispatcher.Dispatch<KeyTypedEvent>(IE_BIND_EVENT_FN(InputManager::OnKeyTypedEvent));
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