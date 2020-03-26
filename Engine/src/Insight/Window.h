#pragma once

#include "ie_pch.h"

#include "Insight/Core.h"
#include "Insight/Events/Event.h"

namespace Insight {

	struct WindowProps
	{
		std::string Title;
		std::string Class;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Insight Engine", const std::string winClass = "IE Class", uint32_t width = 1600, uint32_t height = 900)
			: Title(title), Width(width), Height(height) {}
	};

	class INSIGHT_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		
		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window Attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual bool ProccessWindowMessages() = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVsyncActive() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};

}