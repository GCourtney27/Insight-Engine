#pragma once

#include "Event.h"

namespace Insight {

	class INSIGHT_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height, bool minimized)
			: m_Width(width), m_Height(height), m_Minimized(minimized) {}

		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }
		inline bool GetIsMinimized() const { return m_Minimized; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResisedEvent: " << m_Width << ", " << m_Height << " | Minimized: " << m_Minimized;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		unsigned int m_Width, m_Height;
		bool m_Minimized;
	};

	class INSIGHT_API WindowToggleFullScreenEvent : public Event
	{
	public:
		WindowToggleFullScreenEvent(bool enabled) 
			: m_Enabled(enabled) {}

		inline bool GetFullScreenEnabled() const { return m_Enabled; }

		EVENT_CLASS_TYPE(ToggleWindowFullScreen)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		bool m_Enabled;
	};

	class INSIGHT_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class INSIGHT_API AppTickEvent : public Event
	{
	public:
		AppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class INSIGHT_API AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class INSIGHT_API AppBeginPlayEvent : public Event
	{
	public:
		AppBeginPlayEvent() {}

		EVENT_CLASS_TYPE(AppBeginPlay)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class INSIGHT_API AppEndPlayEvent : public Event
	{
	public:
		AppEndPlayEvent() {}

		EVENT_CLASS_TYPE(AppEndPlay)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class INSIGHT_API AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class INSIGHT_API AppScriptReloadEvent : public Event
	{
	public:
		AppScriptReloadEvent() {}

		EVENT_CLASS_TYPE(AppScriptReload)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class INSIGHT_API SceneSaveEvent : public Event
	{
	public:
		SceneSaveEvent() {}

		EVENT_CLASS_TYPE(SceneSave)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

}