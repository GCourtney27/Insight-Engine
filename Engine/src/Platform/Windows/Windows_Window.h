#pragma once

#include "Insight/Window.h"
#include "Platform/DirectX12/Direct3D12_Context.h"


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
			uint32_t Width, Height;
			bool VSyncEnabled = true;

			EventCallbackFn EventCallback;
		};

	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		virtual void OnUpdate() override;

		inline uint32_t GetWidth() const override { return m_Data.Width; }
		inline uint32_t GetHeight() const override { return m_Data.Height; }

		virtual void* GetNativeWindow() const override;
		void SetWindowsSessionProps(HINSTANCE& hInstance, int nCmdShow) { SetWindowsApplicationInstance(hInstance); SetCmdArgs(nCmdShow); }
		inline HINSTANCE& GetWindowsApplicationReference() const { return *m_WindowsAppInstance; }
		inline HWND& GetWindowHandleReference() { return m_WindowHandle; }

		virtual bool ProccessWindowMessages() override;

		// Window Attributes
		virtual inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVsyncActive() const override;
		virtual bool Init(const WindowProps& props);
	private:
		inline void SetWindowsApplicationInstance(HINSTANCE& hInstance) { m_WindowsAppInstance = &hInstance; }
		inline void SetCmdArgs(int nCmdShow) { m_nCmdShowArgs = nCmdShow; }
		void RegisterWindowClass();
		virtual void Shutdown();
	private:
		RenderingContext* m_RendererContext;

		HWND m_WindowHandle;
		HINSTANCE* m_WindowsAppInstance;
		int m_nCmdShowArgs;
		WindowData m_Data;
		bool m_WindowResizeBegun = false;
		bool m_WindowResizeInProgress = false;

	};

}
