#pragma once

#include "Insight/Core/Window.h"


namespace Insight {

	class WindowsWindow : public Window
	{
	public:
		struct WindowData
		{
			std::string WindowClassName = "Insight Engine Class";
			std::wstring WindowClassName_wide;
			std::string WindowTitle = "Insight Engine";
			std::wstring WindowTitle_wide;
			UINT Width, Height;
			bool VSyncEnabled = true;
			bool FullScreenEnabled = false;

			bool isFirstLaunch = true;

			EventCallbackFn EventCallback;
		};

	public:

		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		virtual void OnUpdate() override;
		virtual void OnFramePreRender() override;
		virtual void OnRender() override;
		virtual void ExecuteDraw() override;
		virtual void Shutdown() override;

		inline uint32_t GetWidth() const override { return m_Data.Width; }
		inline uint32_t GetHeight() const override { return m_Data.Height; }

		virtual void* GetNativeWindow() const override;
		void SetWindowsSessionProps(HINSTANCE& hInstance, int nCmdShow) { SetWindowsApplicationInstance(hInstance); SetCmdArgs(nCmdShow); }
		inline HINSTANCE& GetWindowsApplicationReference() const { return *m_WindowsAppInstance; }
		inline HWND& GetWindowHandleReference() { return m_WindowHandle; }
		inline RECT& GetWindowRect() { return m_WindowRect; }

		virtual void Resize(uint32_t newWidth, uint32_t newHeight, bool isMinimized) override;
		virtual void ToggleFullScreen(bool enabled) override;

		virtual bool ProccessWindowMessages() override;

		// Window Attributes
		virtual inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual void SetVSync(bool enabled) override;
		virtual const bool& IsVsyncActive() const override;
		virtual const bool& IsFullScreenActive() const override;
		virtual bool Init(const WindowProps& props);
	private:
		inline void SetWindowsApplicationInstance(HINSTANCE& hInstance) { m_WindowsAppInstance = &hInstance; }
		inline void SetCmdArgs(int nCmdShow) { m_nCmdShowArgs = nCmdShow; }
		void RegisterWindowClass();
		void SetWindowBounds();
	private:

		HWND m_WindowHandle;
		HINSTANCE* m_WindowsAppInstance;
		int m_nCmdShowArgs;
		RECT m_WindowRect;

		WindowData m_Data;
	};

}
