#pragma once

#include "Insight/Core/Window.h"


namespace Insight {

	class INSIGHT_API Win32Window : public Window
	{
	public:
		Win32Window(const WindowDescription& props);
		virtual ~Win32Window();

		virtual void OnUpdate() override;
		virtual void Shutdown() override;

		virtual void PostInit() override;
		virtual void* GetNativeWindow() const override;
		virtual bool ProccessWindowMessages() override;

		bool SetWindowTitle(const std::string& newText, bool completlyOverride = false) override;
		bool SetWindowTitleFPS(float fps) override;

		inline HINSTANCE& GetWindowsApplicationReference() const { return *m_WindowsAppInstance; }
		inline HWND& GetWindowHandleRef() { return m_hWindow; }
		inline RECT& GetWindowRect() { return m_WindowRect; }

		virtual void CreateMessageBox(const std::wstring& Message, const std::wstring Title) override;

		// Window Attributes
		virtual inline void SetEventCallback(const EventCallbackFn& callback) override { m_EventCallbackFn = callback; }
		virtual bool Init(HINSTANCE& hInstance, int nCmdShow, LPWSTR CmdLineArgs);
		

		HMENU& GetGraphicsSubmenu() { return m_hGraphicsVisualizeSubMenu; }
		HMENU& GetEditorSubmenu() { return m_hEditorSubMenu; }
		HMENU& GetContextSubmenu() { return m_hContextMenu; }

		EventCallbackFn& GetEventCallbackFn() { return m_EventCallbackFn; }

	private:
		void RegisterWindowClass();
		void InitializeMenuBar();
		void InitializeContextMenu();
		LPCTSTR GetLastWindowsError();
	private:
		EventCallbackFn m_EventCallbackFn;
		
		HWND	m_hWindow;
		HMENU	m_hContextMenu;
		HMENU	m_hMenuBar;
		HMENU	m_hFileSubMenu;
		HMENU	m_hEditSubMenu;
		HMENU	m_hEditorSubMenu;
		HMENU	m_hGraphicsSubMenu;
		HMENU	m_hGraphicsVisualizeSubMenu;
		HMENU	m_hGraphicsCurrentRenderContextSubMenu;

		HINSTANCE* m_WindowsAppInstance;
		int m_NumCmdLineArgs;
		LPWSTR m_CmdLineArgs;
		RECT m_WindowRect;

	};

}
