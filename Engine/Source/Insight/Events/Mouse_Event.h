#pragma once

#include "Event.h"

namespace Insight {

	class INSIGHT_API MouseMovedEvent : public Event, public InputEvent
	{
	public:
		MouseMovedEvent(float x, float y, KeyMapCode KeyMapCode)
			: InputEvent(KeyMapCode, InputEventType_Moved), m_MouseX(x), m_MouseY(y) {}

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
			EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_MouseX, m_MouseY;
	};

	class INSIGHT_API MouseRawMoveEvent : public Event, public InputEvent
	{
	public:
		MouseRawMoveEvent(int x, int y, KeyMapCode KeyMapCode, InputEventType EventType)
			: InputEvent(KeyMapCode, EventType), m_MouseX(x), m_MouseY(y) {}

		inline int GetX() const { return m_MouseX; }
		inline int GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(RawMouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		int m_MouseX, m_MouseY;
	};

	class INSIGHT_API MouseScrolledEvent : public Event, public InputEvent
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset, KeyMapCode KeyMapCode, InputEventType EventType)
			: InputEvent(KeyMapCode, EventType), m_XOffset(xOffset), m_YOffset(yOffset) {}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << m_XOffset << ", " << m_YOffset;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_XOffset, m_YOffset;
	};

	class INSIGHT_API MouseButtonEvent : public Event, public InputEvent
	{
	public:

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	protected:
		MouseButtonEvent(KeyMapCode KeyMapCode, InputEventType EventType)
			: InputEvent(KeyMapCode, EventType) {}

	};

	class INSIGHT_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(KeyMapCode KeyMapCode)
			: MouseButtonEvent(KeyMapCode, InputEventType_Pressed) {}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class INSIGHT_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(KeyMapCode KeyMapCode)
			: MouseButtonEvent(KeyMapCode, InputEventType_Released) {}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

}