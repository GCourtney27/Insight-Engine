#pragma once

#include <ie_pch.h>

#include <Insight/Core.h>
#include "Insight/Events/Event.h"

class RenderingContext;

namespace Insight {

	struct WindowProps
	{
		std::string Title;
		std::string Class;
		UINT Width;
		UINT Height;

		WindowProps(const std::string& title = "Insight Editor", const std::string winClass = "IE Class", UINT width = 1700, UINT height = 1000)
			: Title(title), Width(width), Height(height) {}
	};

	class INSIGHT_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		
		virtual ~Window() {}

		virtual void OnUpdate(const float& deltaTime) = 0;
		virtual void OnFramePreRender() = 0;
		virtual void OnRender() = 0;
		virtual void ExecuteDraw() = 0;
		virtual void Shutdown() = 0;
		virtual void EndFrame() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual bool SetWindowTitle(const std::string& newText, bool completlyOverride = false) = 0;
		virtual bool SetWindowTitleFPS(float fps) = 0;

		virtual void Resize(uint32_t newWidth, uint32_t newHeight, bool isMinimized) = 0;
		virtual void ToggleFullScreen(bool enabled) = 0;

		// Window Attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual bool ProccessWindowMessages() = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual const bool& IsFullScreenActive() const = 0;
		virtual const bool& IsVsyncActive() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());

	protected:

	};

}