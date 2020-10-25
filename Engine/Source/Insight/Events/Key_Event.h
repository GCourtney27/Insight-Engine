#pragma once

#include "Event.h"

namespace Insight {

	class INSIGHT_API KeyEvent : public Event, public InputEvent
	{
	public:

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(KeymapCode KeyCode, InputEventType Status)
			: InputEvent(KeyCode, Status) {}

	};

	class INSIGHT_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(KeymapCode keycode, int repeatCount)
			: KeyEvent(keycode, InputEventType_Pressed), m_RepeatCount(repeatCount)
		{}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyMapCode << " (" << m_RepeatCount << " repeates)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
	};

	class INSIGHT_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(KeymapCode Keycode)
			: KeyEvent(Keycode, InputEventType_Released) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyMapCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class INSIGHT_API KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(KeymapCode Keycode)
			: KeyEvent(Keycode, InputEventType_Typed) {}


		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyMapCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};

}