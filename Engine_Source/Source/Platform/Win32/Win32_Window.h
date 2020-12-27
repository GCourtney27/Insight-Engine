#pragma once

#include "Insight/Core/Window.h"

#if defined (IE_PLATFORM_BUILD_WIN32)

namespace Insight {

	
	struct Win32WindowDescription : public WindowDescription
	{
		HINSTANCE AppInstance;

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

		virtual std::pair<uint32_t, uint32_t> GetDPI() const override
		{
			UINT DPI = ::GetDpiForWindow(m_hWindow);
			return std::make_pair(DPI, DPI);
		}


		// Window Attributes
		virtual bool Init();

		HMENU& GetGraphicsSubmenu() { return m_hGraphicsVisualizeSubMenu; }
		HMENU& GetEditorSubmenu() { return m_hEditorSubMenu; }
		HMENU& GetContextSubmenu() { return m_hContextMenu; }


	private:
		void RegisterWindowClass();
		void InitializeMenuBar();
		void InitializeContextMenu();
		LPCTSTR GetLastWindowsError();
	private:
		
		HWND	m_hWindow;

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

}
#endif // IE_PLATFORM_BUILD_WIN32
