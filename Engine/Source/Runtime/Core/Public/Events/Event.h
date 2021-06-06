// Copyright Insight Interactive. All Rights Reserved.
#pragma once
#include <Engine_pch.h>

#include "EngineDefines.h"
#include "Core/Public/Input/KeyCodes.h"

namespace Insight {

	enum class EEventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved, ToggleWindowFullScreen, ShaderReload,
		AppBeginPlay, AppEndPlay, AppTick, AppUpdate, AppRender, AppScriptReload, AppSuspending, AppResuming,
		SceneSave,
		KeyPressed, KeyReleased, KeyTyped, KeyHeld,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, RawMouseMoved, MouseScrolled,
		PhysicsCollisionEvent, WorldTranslationEvent
	};

	enum EEventCategory
	{
		None = 0,
		EventCategoryApplication	= 1 << 0,
		EventCategoryInput			= 1 << 1,
		EventCategoryKeyboard		= 1 << 2,
		EventCategoryMouse			= 1 << 3,
		EventCategoryMouseButton	= 1 << 4,
		EventCategoryPhysics		= 1 << 5,
		EventCategoryTranslation	= 1 << 6
	};

#define EVENT_CLASS_TYPE(type) static EEventType GetStaticType() { return EEventType::##type; }\
								virtual EEventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class INSIGHT_API Event
	{
		friend class EventDispatcher;
	public:
		virtual EEventType GetEventType() const = 0;
		virtual const char * GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool Handled() const { return m_Handled; }

		inline bool IsInCategory(EEventCategory category)
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
		inline EInputEventType GetEventType() const { return m_Status; }
		
		std::string ToString() const
		{
			std::stringstream ss;
			ss << "Input Event: (Button) " << m_KeyMapCode << " (Status) " << m_Status;
			return ss.str();
		}
	protected:
		InputEvent(KeyMapCode KeyMapCode, EInputEventType Status)
			: m_KeyMapCode(KeyMapCode), m_Status(Status) 
		{}

		KeyMapCode m_KeyMapCode;
		EInputEventType m_Status;
	};

	class INSIGHT_API RendererEvent : public Event
	{
	public:
		RendererEvent() = default;
		~RendererEvent() = default;

		virtual EEventType GetEventType() const override { return EEventType::AppRender; }
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