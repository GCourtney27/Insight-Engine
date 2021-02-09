// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include "Event.h"

namespace Insight {

	class INSIGHT_API KeyEvent : public Event, public InputEvent
	{
	public:

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(KeyMapCode KeyCode, EInputEventType Status)
			: InputEvent(KeyCode, Status) {}

	};

	class INSIGHT_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(KeyMapCode keycode, int repeatCount, float MoveDelta = 1.0f)
			: KeyEvent(keycode, IET_Pressed), m_RepeatCount(repeatCount), m_MoveDelta(MoveDelta)
		{}

		inline int GetRepeatCount() const { return m_RepeatCount; }
		inline float GetMoveDelta() const { return m_MoveDelta; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyMapCode << " (" << m_RepeatCount << " repeates)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
		float m_MoveDelta;
	};

	class INSIGHT_API KeyHeldEvent : public KeyEvent
	{
	public:
		KeyHeldEvent(KeyMapCode Keycode)
			: KeyEvent(Keycode, IET_Held) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyHeldEvent: " << m_KeyMapCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyHeld)
	};

	class INSIGHT_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(KeyMapCode Keycode)
			: KeyEvent(Keycode, IET_Released) {}

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
		KeyTypedEvent(KeyMapCode Keycode)
			: KeyEvent(Keycode, IET_Typed) {}


		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyMapCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};

}