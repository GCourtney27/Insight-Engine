#pragma once

#include "Runtime/Core/Window.h"


namespace Insight {

#if IE_PLATFORM_BUILD_WIN32

	/*
		Function signature for a callback that can process custom IDM_*
		commands from windows resource files.

		Acceptable function sugnature: LRESULT _stdcall MyMethod(int&)
	*/
	typedef LRESULT (*OutLResInIntRefMethod_t)(int&);
	
	struct Win32WindowDescription : public WindowDescription
	{
		/*
			The windows app instance for this program.
		*/
		HINSTANCE		AppInstance;
		
		/*
			Window accelerator table.
		*/
		HACCEL			AccelerationTable;
		
		/*
			Name of the menu bar.
		*/
		LPCWSTR	MenuBarName;
		
		/*
			The icon for the window of the application.
		*/
		HICON Icon;

		/*
			The mouse corsor for the application.
		*/
		HCURSOR Cursor;

		/*
			Custom callback used to handle accelerator tabale commands.
		*/
		OutLResInIntRefMethod_t UserAccelCallback;

		template <typename ... WindowDescriptionArgs>
		Win32WindowDescription(HINSTANCE hInstance, WindowDescriptionArgs ... args)
			: AppInstance(hInstance), WindowDescription(args...) {}
	};

	class INSIGHT_API Win32Window : public Window
	{
	public:
		Win32Window(const Win32WindowDescription& props);
		virtual ~Win32Window();

		virtual void OnUpdate() override;
		virtual void Shutdown() override;

		virtual void PostInit() override;
		virtual void* GetNativeWindow() const override;
		virtual bool ProccessWindowMessages() override;

		virtual InputEventType GetAsyncKeyState(KeyMapCode Key) const override;
		bool SetWindowTitle(const std::string& newText, bool completlyOverride = false) override;
		bool SetWindowTitleFPS(float fps) override;

		inline HINSTANCE GetWindowsApplicationReference() const { return m_WindowsAppInstance; }
		inline HWND& GetWindowHandleRef() { return m_hWindow; }
		inline RECT& GetWindowRect() { return m_WindowRect; }

		virtual void CreateMessageBox(const std::wstring& Message, const std::wstring& Title) override;

		static OutLResInIntRefMethod_t MakeAccelCallback(void* Fn) { return (OutLResInIntRefMethod_t)Fn; }
		OutLResInIntRefMethod_t GetCustomCallback() { return m_CustomCallback; }

		// Window Attributes
		virtual bool Init();

		HMENU& GetGraphicsSubmenu() { return m_hGraphicsVisualizeSubMenu; }
		HMENU& GetEditorSubmenu() { return m_hEditorSubMenu; }
		HMENU& GetContextSubmenu() { return m_hContextMenu; }

	protected:
		virtual inline void SetNativeWindowDPI() override
		{
			m_DPI = static_cast<float>(::GetDpiForWindow(m_hWindow));
		}

	private:
		void RegisterWindowClass();
		void InitializeMenuBar();
		void InitializeContextMenu();
		LPCTSTR GetLastWindowsError();

	private:
		
		HWND	m_hWindow;
		HACCEL	m_hAccelerationTable;
		LPCWSTR m_MenuBarName;
		OutLResInIntRefMethod_t m_CustomCallback;

		HICON m_Icon;
		HCURSOR m_Cursor;


		HMENU	m_hContextMenu;
		HMENU	m_hMenuBar;
		HMENU	m_hFileSubMenu;
		HMENU	m_hEditSubMenu;
		HMENU	m_hEditorSubMenu;
		HMENU	m_hGraphicsSubMenu;
		HMENU	m_hGraphicsVisualizeSubMenu;
		HMENU	m_hGraphicsCurrentRenderContextSubMenu;

		HINSTANCE m_WindowsAppInstance;
		int m_NumCmdLineArgs;
		std::wstring m_CmdLineArgs;
		RECT m_WindowRect;

	};

#endif // IE_PLATFORM_BUILD_WIN32
}
