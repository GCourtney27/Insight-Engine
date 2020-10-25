#include "Engine_pch.h"

#include "Input_Dispatcher.h"


namespace Insight {


	InputDispatcher* InputDispatcher::s_Instance = nullptr;

	void InputDispatcher::UpdateInputs()
	{
		for (uint32_t i = 0; i < m_AxisMappings.size(); i++)
		{
			SHORT KeyState = ::GetAsyncKeyState(m_AxisMappings[i].MappedKeycode);
			bool Pressed = (BIT_SHIFT(15)) & KeyState;
			if (Pressed)
			{
				KeyPressedEvent e(m_AxisMappings[i].MappedKeycode, 0);
				ProcessInputEvent(e);
			}
		}
	}

	void InputDispatcher::ProcessInputEvent(Event& e)
	{
		EventDispatcher Dispatcher(e);
		// Mouse Buttons
		//Dispatcher.Dispatch<MouseButtonPressedEvent>(IE_BIND_EVENT_FN(InputManager::OnMouseButtonPressedEvent));
		//Dispatcher.Dispatch<MouseButtonReleasedEvent>(IE_BIND_EVENT_FN(InputManager::OnMouseButtonReleasedEvent));

		// Mouse Moved
		//Dispatcher.Dispatch<MouseMovedEvent>(IE_BIND_EVENT_FN(InputManager::OnMouseMovedEvent));
		Dispatcher.Dispatch<MouseMovedEvent>(IE_BIND_EVENT_FN(InputDispatcher::DispatchMouseMoveEvent));
		Dispatcher.Dispatch<MouseButtonPressedEvent>(IE_BIND_EVENT_FN(InputDispatcher::DispatchActionEvent));
		Dispatcher.Dispatch<MouseButtonReleasedEvent>(IE_BIND_EVENT_FN(InputDispatcher::DispatchActionEvent));



		// Mouse Scroll
		//Dispatcher.Dispatch<MouseScrolledEvent>(IE_BIND_EVENT_FN(InputManager::OnMouseScrollEvent));

		// Key Pressed
		Dispatcher.Dispatch<KeyPressedEvent>(IE_BIND_EVENT_FN(InputDispatcher::DispatchKeyPressEvent));
		Dispatcher.Dispatch<KeyPressedEvent>(IE_BIND_EVENT_FN(InputDispatcher::DispatchActionEvent));
		Dispatcher.Dispatch<KeyReleasedEvent>(IE_BIND_EVENT_FN(InputDispatcher::DispatchActionEvent));

		// Key Typed
		//Dispatcher.Dispatch<KeyTypedEvent>(IE_BIND_EVENT_FN(InputManager::OnKeyTypedEvent));
	}

	void InputDispatcher::RegisterAxisCallback(const char* Name, EventInputAxisFn Callback)
	{
		m_AxisCallbacks[Name].push_back(Callback);
	}

	void InputDispatcher::RegisterActionCallback(const char* Name, InputEventType EventType, EventInputActionFn Callback)
	{
		m_ActionCallbacks[{Name, EventType}].push_back(Callback);
	}

	bool InputDispatcher::DispatchKeyPressEvent(KeyPressedEvent& e)
	{
		for (AxisMapping& Axis : m_AxisMappings)
		{
			// Find the key in the axis map.
			if (Axis.MappedKeycode == e.GetKeyCode())
			{
				// Use the keycode as the key into the axis map to find 
				// the callbacks associated with it.
				auto Callbacks = &m_AxisCallbacks[Axis.Hint];
				for (EventInputAxisFn Callback : *Callbacks)
				{
					// Call the callbacks.
					Callback(Axis.Scale);
				}
			}
		}
		return false;
	}

	bool InputDispatcher::DispatchMouseMoveEvent(MouseMovedEvent& e)
	{
		for(AxisMapping& Axis : m_AxisMappings)
		{
			// Find the key in the axis map.
			if (Axis.MappedKeycode == e.GetKeyCode())
			{
				float MoveFactor = 0.0f;
				if (e.GetKeyCode() == KeymapCode_Mouse_MoveX) MoveFactor = e.GetX();
				if (e.GetKeyCode() == KeymapCode_Mouse_MoveY) MoveFactor = e.GetY();

				// Use the keycode as the key into the axis map to find 
				// the callbacks associated with it.
				auto Callbacks = &m_AxisCallbacks[Axis.Hint];
				for (EventInputAxisFn Callback : *Callbacks)
				{
					// Invoke the callbacks.
					Callback(MoveFactor);
				}
			}
		}
		return true;
	}

	bool InputDispatcher::DispatchActionEvent(InputEvent& e)
	{
		for (ActionMapping& Action : m_ActionMappings)
		{
			if (Action.MappedKeycode == e.GetKeyCode())
			{
				if (e.GetEventType() == InputEventType_Released)
				{
					Action.CanDispatch = true;
				}

				if (Action.CanDispatch)
				{
					auto Callbacks = &m_ActionCallbacks[{Action.Hint, e.GetEventType()}];
					for (EventInputActionFn Callback : *Callbacks)
					{
						// Invoke the callbacks.
						Callback();
					}
				}

				if (e.GetEventType() == InputEventType_Pressed)
				{
					Action.CanDispatch = false;
				}
			}
		}
		return true;
	}

}
