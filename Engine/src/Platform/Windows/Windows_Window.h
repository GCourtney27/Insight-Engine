#pragma once

#include "Insight/Window.h"

#include <Windows.h>

namespace Insight {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		virtual void OnUpdate() override;

		inline uint32_t GetWidth() const override { return m_Data.Width; }
		inline uint32_t GetHeight() const override { return m_Data.Height; }

		virtual void* GetNativeWindow() const override;


#ifdef IE_PLATFORM_WINDOWS
		inline HINSTANCE& GetWindowsApplicationReference() const { return *m_WindowsApplicationInstance; }
		void SetWindowsApplicationInstance(HINSTANCE& hInstance) { m_WindowsApplicationInstance = &hInstance; }
		void SetCmdArgs(int nCmdShow) { m_nCmdShowArgs = nCmdShow; }
#endif
		virtual bool ProccessWindowMessages() override;
		void CreateConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns);

		virtual void SetEventCallback(const EventCallbackFn& callback) override;
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVsyncActive() const override;
		virtual void Init(const WindowProps& props);
	private:
		virtual void Shutdown();
	private:
		struct WindowData
		{
			LPCSTR WindowClassName = "Insight Engine Class";
			LPCSTR WindowTitle = "Insight Engine";
			uint32_t Width, Height;
			bool VSyncEnabled;

			EventCallbackFn EventCallback;
		};
		HWND m_WindowHandle;
		HINSTANCE* m_WindowsApplicationInstance;
		int m_nCmdShowArgs;
		WindowData m_Data;

		// Console
		HWND m_ConsoleWindowHandle;
		HMENU m_ConsoleWindowHMenu;

	};

}
