#pragma once

#include <Insight/Core.h>

#include "Insight/Core/Window.h"

namespace Insight {

	class INSIGHT_API UWPWindow : public Window
	{
	public:
		struct WindowData
		{
			std::string WindowClassName = "Insight Engine Class";
			std::wstring WindowClassName_wide;
			std::string WindowTitle = "Insight Editor";
			std::wstring WindowTitle_wide;
			UINT Width, Height;
			bool VSyncEnabled = true;
			bool FullScreenEnabled = false;
			bool EditorUIEnabled = true;
			bool IsFirstLaunch = true;
			UWPWindow* pWindow;

			EventCallbackFn EventCallback;
		};
	public:
		UWPWindow(const WindowDescription& props);
		virtual ~UWPWindow();

		bool Init(::IUnknown* pWindow)
		{
			m_pWindow = pWindow;
			return true;
		}

		virtual void OnUpdate() override {}
		virtual void Shutdown() override {}

		virtual void PostInit() override {}

		virtual void* GetNativeWindow() const override { return static_cast<void*>(m_pWindow); }
		virtual bool SetWindowTitle(const std::string& newText, bool completlyOverride = false) override { return true; }
		virtual bool SetWindowTitleFPS(float fps) override { return true; }

		virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual bool ProccessWindowMessages() override { return true; }

		virtual void CreateMessageBox(const std::wstring& Message, const std::wstring Title) override {}

	private:
		WindowData m_Data;
		::IUnknown* m_pWindow;
	};
}
