// Copyright Insight Interactive. All Rights Reserved.
#pragma once
#include <Engine_pch.h>

#include <Insight/Core.h>
#include "Insight/Input/Key_Codes.h"

namespace Insight {

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved, ToggleWindowFullScreen, ShaderReload,
		AppBeginPlay, AppEndPlay, AppTick, AppUpdate, AppRender, AppScriptReload,
		SceneSave,
		KeyPressed, KeyReleased, KeyTyped, KeyHeld,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, RawMouseMoved, MouseScrolled,
		PhysicsCollisionEvent, WorldTranslationEvent
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT_SHIFT(0),
		EventCategoryInput = BIT_SHIFT(1),
		EventCategoryKeyboard = BIT_SHIFT(2),
		EventCategoryMouse = BIT_SHIFT(3),
		EventCategoryMouseButton = BIT_SHIFT(4),
		EventCategoryPhysics = BIT_SHIFT(5),
		EventCategoryTranslation = BIT_SHIFT(6)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class INSIGHT_API Event
	{
		friend class EventDispatcher;
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char * GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool Handled() const { return m_Handled; }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	protected:
		bool m_Handled = false;
	};

	class INSIGHT_API InputEvent
	{
	public:
		inline KeyMapCode GetKeyCode() const { return m_KeyMapCode; }
		inline InputEventType GetEventType() const { return m_Status; }
		
		std::string ToString() const
		{
			std::stringstream ss;
			ss << "Input Event: (Button) " << m_KeyMapCode << " (Status) " << m_Status;
			return ss.str();
		}
	protected:
		InputEvent(KeyMapCode KeyMapCode, InputEventType Status)
			: m_KeyMapCode(KeyMapCode), m_Status(Status) 
		{}

		KeyMapCode m_KeyMapCode;
		InputEventType m_Status;
	};

	class INSIGHT_API RendererEvent : public Event
	{
	public:
		RendererEvent() = default;
		~RendererEvent() = default;

		virtual EventType GetEventType() const override { return EventType::AppRender; }
		virtual const char* GetName() const override { return "Default Render Event Type"; }
		virtual int GetCategoryFlags() const override { return -1; }
	};

	class INSIGHT_API EventDispatcher
	{
		template<typename Event>
		using EventFn = std::function<bool(Event&)>;

	public:
		EventDispatcher(Event& event)
			: m_Event(event) {}

		template<typename Func>
		bool Dispatch(EventFn<Func> func)
		{
			if (m_Event.GetEventType() == Func::GetStaticType())
			{
				m_Event.m_Handled = func(*(Func*)&m_Event);
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

}