#include "Engine_pch.h"

#include "Input_Dispatcher.h"


namespace Insight {


	InputDispatcher* InputDispatcher::s_Instance = nullptr;

	void InputDispatcher::Update()
	{
		for (uint32_t i = 0; i < m_AxisMappings.size(); i++)
		{
			SHORT KeyState = GetAsyncKeyState(m_AxisMappings[i].Keycode);
			bool Pressed = (1 << 15) & KeyState;
			if (Pressed)
			{
				ProcessInputEvent(KeyPressedEvent(m_AxisMappings[i].Keycode, 0));
			}
		}
	}

	void InputDispatcher::ProcessInputEvent(Event& e)
	{
		EventDispatcher Dispatcher(e);
		// Mouse Buttons
		//Dispatcher.Dispatch<MouseButtonPressedEvent>(IE_BIND_EVENT_FN(InputManager::OnMouseButtonPressedEvent));
		//Dispatcher.Dispatch<MouseButtonReleasedEvent>(IE_BIND_EVENT_FN(InputManager::OnMouseButtonReleasedEvent));
		//// Mouse Moved
		//Dispatcher.Dispatch<MouseMovedEvent>(IE_BIND_EVENT_FN(InputManager::OnMouseMovedEvent));
		//Dispatcher.Dispatch<MouseRawMoveEvent>(IE_BIND_EVENT_FN(InputManager::OnRawMouseMoveEvent));
		//// Mouse Scroll
		//Dispatcher.Dispatch<MouseScrolledEvent>(IE_BIND_EVENT_FN(InputManager::OnMouseScrollEvent));

		// Key Pressed
		Dispatcher.Dispatch<KeyPressedEvent>(IE_BIND_EVENT_FN(InputDispatcher::DispatchKeyPressEvent));
		Dispatcher.Dispatch<KeyReleasedEvent>(IE_BIND_EVENT_FN(InputDispatcher::DispatchKeyReleaseEvent));

		// Key Typed
		//Dispatcher.Dispatch<KeyTypedEvent>(IE_BIND_EVENT_FN(InputManager::OnKeyTypedEvent));
	}

	void InputDispatcher::RegisterAxisCallback(const char* Name, EventInputAxisFn Callback)
	{
		m_AxisCallbacks[Name].push_back(Callback);
	}

	void InputDispatcher::RegisterActionCallback(const char* Name, InputEventType EventType, EventInputActionFn Callback)
	{
	}

	bool InputDispatcher::DispatchKeyPressEvent(KeyPressedEvent& e)
	{
		int KeyCode = e.GetKeyCode();


		for (uint32_t i = 0; i < m_AxisMappings.size(); i++)
		{
			if (m_AxisMappings[i].Keycode == KeyCode)
			{
				std::string_view AxisMappingName = m_AxisMappings[i].Hint;
				std::vector<EventInputAxisFn>* Callbacks = &m_AxisCallbacks[AxisMappingName.data()];
				for (EventInputAxisFn Callback : *Callbacks)
				{
					Callback(m_AxisMappings[i].Scale);
				}

			}
		}
		return false;
	}

	bool InputDispatcher::DispatchKeyReleaseEvent(KeyReleasedEvent& e)
	{
		return false;
	}

}
