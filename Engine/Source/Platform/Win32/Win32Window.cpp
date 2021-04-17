// Copyright Insight Interactive. All Rights Reserved.
#include <Engine_pch.h>

// The contents can only be accessed on a Win32 platform.
// If it is not present than just compile it out.
#if IE_PLATFORM_BUILD_WIN32

#include "Win32Window.h"

#include "Runtime/Core/Application.h"
#include "Runtime/Graphics/Renderer.h"

#include "Runtime/Core/Log.h"
#include "Runtime/Utilities/StringHelper.h"
//#include "Platform/Win32/Resources/Resource.h"

#include "Runtime/Events/KeyEvent.h"
#include "Runtime/Events/MouseEvent.h"
#include "Runtime/Events/ApplicationEvent.h"
#include "Runtime/Core/Exception.h"


namespace Insight {


	Win32Window::Win32Window(const Win32WindowDescription& Desc)
	{
		IE_ASSERT(Desc.Class != L"", "Window class cannot be blank.");
		IE_ASSERT(Desc.Title != L"", "Window title cannot be blank.");
		IE_ASSERT(Desc.Width > 0 && Desc.Height > 0, "Width and height of window cannot be 0.");

		m_WindowTitle			= Desc.Title;
		m_WindowClassName		= Desc.Class;
		m_LogicalWidth			= Desc.Width;
		m_LogicalHeight			= Desc.Height;
		m_MenuBarName			= Desc.MenuBarName;
		m_WindowsAppInstance	= Desc.AppInstance;
		m_hAccelerationTable	= Desc.AccelerationTable;
		m_NumCmdLineArgs		= Desc.NumCmdArgs;
		m_CmdLineArgs			= Desc.CmdArgs;
		m_EventCallbackFn		= Desc.EventCallbackFunction;
		m_CustomCallback		= Desc.UserAccelCallback;
		m_Icon					= Desc.Icon;
		m_Cursor				= Desc.Cursor;

		Init();
	}

	LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		Win32Window& pWindow = *(Win32Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		switch (uMsg) 
		{
		// Application
		case (WM_NCCREATE):
		{
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Win32Window* data = reinterpret_cast<Win32Window*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data));
			break;
		}
		case (WM_QUIT):
		case (WM_DESTROY):
		{
			WindowCloseEvent event;
			pWindow.GetEventCallbackFn()(event);
			return 0;
		}
		// Mouse Input
		case (WM_MOUSEMOVE):
		{
			MouseMovedEvent event(LOWORD(lParam), HIWORD(lParam), (KeyMapCode)(KeyMapCode_Mouse_MoveX | KeyMapCode_Mouse_MoveY));
			pWindow.GetEventCallbackFn()(event);
			return 0;
		}
		case (WM_MOUSEWHEEL):
		{
			float yOffset = GET_WHEEL_DELTA_WPARAM(wParam) / 120.0f;
			MouseScrolledEvent event(0.0f, yOffset, KeyMapCode_Mouse_Wheel_Up, IET_Moved);
			pWindow.GetEventCallbackFn()(event);
			return 0;
		}
		case (WM_MOUSEHWHEEL):
		{
			float xOffset = GET_WHEEL_DELTA_WPARAM(wParam) / 120.0f;
			MouseScrolledEvent event(xOffset, 0.0f, (KeyMapCode)(KeyMapCode_Mouse_Wheel_Left | KeyMapCode_Mouse_Wheel_Right), IET_Moved);
			pWindow.GetEventCallbackFn()(event);
			return 0;
		}
		case (WM_LBUTTONDOWN):
		{
			MouseButtonPressedEvent event(KeyMapCode_Mouse_Button_Left);
			pWindow.GetEventCallbackFn()(event);
			return 0;
		}
		case (WM_LBUTTONUP):
		{
			MouseButtonReleasedEvent event(KeyMapCode_Mouse_Button_Left);
			pWindow.GetEventCallbackFn()(event);
			return 0;
		}
		case (WM_RBUTTONDOWN):
		{
			MouseButtonPressedEvent event(KeyMapCode_Mouse_Button_Right);
			pWindow.GetEventCallbackFn()(event);

			// WIP Right click context menu
			//RECT clientRect = {};
			//POINT Point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			//TrackPopupMenu(pWindow.GetEventCallbackFn().hContextMenu, TPM_LEFTALIGN | TPM_TOPALIGN, Point.x, Point.y, 0, hWnd, &clientRect);

			return 0;
		}
		case (WM_RBUTTONUP):
		{
			MouseButtonReleasedEvent event(KeyMapCode_Mouse_Button_Right);
			pWindow.GetEventCallbackFn()(event);
			return 0;
		}
		case (WM_MBUTTONDOWN):
		{
			MouseButtonPressedEvent event(KeyMapCode_Mouse_Button_Middle);
			pWindow.GetEventCallbackFn()(event);
			return 0;
		}
		case (WM_MBUTTONUP):
		{
			MouseButtonReleasedEvent event(KeyMapCode_Mouse_Button_Middle);
			pWindow.GetEventCallbackFn()(event);
			return 0;
		}
		// Keyboard Input
		case (WM_CHAR):
		{
			KeyTypedEvent event((KeyMapCode)((char)wParam));
			pWindow.GetEventCallbackFn()(event);
			return 0;
		}
		case (WM_KEYDOWN):
		{
			// Debug force engine close Escape key
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
				WindowCloseEvent event;
				pWindow.GetEventCallbackFn()(event);
				return 0;
			}

			if (wParam == VK_F11)
			{
				pWindow.GetEventCallbackFn()(WindowToggleFullScreenEvent{});
			}
			KeyPressedEvent event((KeyMapCode)((char)wParam), 0);
			pWindow.GetEventCallbackFn()(event);
			return 0;
		}
		case (WM_KEYUP):
		{
			KeyReleasedEvent event((KeyMapCode)((char)wParam));
			pWindow.GetEventCallbackFn()(event);
			return 0;
		}
		// Aplication Events
		case (WM_DPICHANGED):
		{
			pWindow.SetDPI(HIWORD(wParam));
			return 0;
		}
		case (WM_COMPACTING):
		{
			IE_LOG(Warning, TEXT("System memory is low!"));
			return 0;
		}
		case (WM_EXITSIZEMOVE):
		{
			RECT clientRect = {};
			GetClientRect(hWnd, &clientRect);
			WindowResizeEvent event(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, wParam == SIZE_MINIMIZED);
			pWindow.GetEventCallbackFn()(event);

			IE_LOG(Log, TEXT("Window size has changed"));
			return 0;
		}
		case (WM_SIZE):
		{
			/*static bool IsFirstLaunch = true;
			if (IsFirstLaunch) {
				IsFirstLaunch = false;
				return 0;
			}
			RECT clientRect = {};
			GetClientRect(hWnd, &clientRect);
			WindowResizeEvent event(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, wParam == SIZE_MINIMIZED);
			pWindow.GetEventCallbackFn()(event);*/
			return 0;
		}
		case (WM_INPUT):
		{
			UINT DataSize;
			GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &DataSize, sizeof(RAWINPUTHEADER));
			
			if (DataSize > 0)
			{
				std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(DataSize);
				if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &DataSize, sizeof(RAWINPUTHEADER)) == DataSize)
				{
					RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
					if (raw->header.dwType == RIM_TYPEMOUSE)
					{
						if (raw->data.mouse.lLastX != 0.0f)
						{
							MouseMovedEvent event((float)raw->data.mouse.lLastX, 0.0f, KeyMapCode_Mouse_MoveX);
							pWindow.GetEventCallbackFn()(event);
						}
						if (raw->data.mouse.lLastY != 0.0f)
						{
							MouseMovedEvent event(0.0f, (float)raw->data.mouse.lLastY, KeyMapCode_Mouse_MoveY);
							pWindow.GetEventCallbackFn()(event);
						}
					}
				}

			}
			break;
		}
		case (WM_DROPFILES):
		{
			/*WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			UINT iFile;
			LPSTR lpszFile;
			UINT cch;
			DragQueryFileA((HDROP)wParam, iFile, lpszFile, cch);
			IE_LOG(Log, TEXT("File dropped on window"));*/
		}
		// Menu Bar Events
		case (WM_COMMAND):
		{
			// Process any custom commands the user puts in using IDM_*
			int wmId = LOWORD(wParam);
			auto Fn = pWindow.GetCustomCallback();
			if (Fn) Fn(wmId);

			break;
		}
		default:
			break;
		}


		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	bool Win32Window::Init()
	{
		HRESULT hr = ::CoInitialize(NULL);
		if (FAILED(hr)){
			IE_LOG(Error, TEXT("Failed to initialize COM library."));
		}

		static bool RIDInitialized = false;
		if (RIDInitialized == false) {

			RAWINPUTDEVICE RID;

			RID.usUsagePage = 0x01; // Mouse
			RID.usUsage = 0x02;
			RID.dwFlags = 0;
			RID.hwndTarget = NULL;

			if (RegisterRawInputDevices(&RID, 1, sizeof(RID)) == FALSE)
			{
				IE_LOG(Error, TEXT("Failed to register raw input devices. Error: %s"), Platform::GetLastPlatformError());
				return false;
			}
			RIDInitialized = true;
		}

		RegisterWindowClass();
		
		// Center the window on the users monitor
		int centerScreenX = ::GetSystemMetrics(SM_CXSCREEN) / 2 - m_LogicalWidth / 2;
		int centerScreenY = ::GetSystemMetrics(SM_CYSCREEN) / 2 - m_LogicalHeight / 2;
		m_WindowRect.left = centerScreenX;
		m_WindowRect.top = centerScreenY + 35;
		m_WindowRect.right = m_WindowRect.left + m_LogicalWidth;
		m_WindowRect.bottom = m_WindowRect.top + m_LogicalHeight;
		::AdjustWindowRect(&m_WindowRect, WS_OVERLAPPEDWINDOW | WS_EX_ACCEPTFILES, FALSE);

		// Create the main window for the engine/game
		m_hWindow = ::CreateWindowExW(
			WS_EX_ACCEPTFILES,						// Window Styles
			m_WindowClassName.c_str(),				// Window Class
			m_WindowTitle.c_str(),					// Window Title
			WS_OVERLAPPEDWINDOW,					// Window Style

			m_WindowRect.left,							// Start X
			m_WindowRect.top,							// Start Y
			m_WindowRect.right - m_WindowRect.left,		// Width
			m_WindowRect.bottom - m_WindowRect.top,		// Height

			NULL,					// Parent window
			NULL,					// Menu
			m_WindowsAppInstance,	// Current Windows program application instance passed from WinMain
			this					// Additional application data
		);

		if (m_hWindow == NULL) {
			IE_LOG(Critical, TEXT("Unable to create Windows window."));
			IE_LOG(Critical, TEXT("    Error: %s"), Platform::GetLastPlatformError());
			throw ieException(TEXT("Fatal Error: Failed to initialize Win32 window. Handle returned nullptr from Windows API. Window description may have contained invalid parameters."));
		}

		::ShowWindow(m_hWindow, m_NumCmdLineArgs);
		::SetForegroundWindow(m_hWindow);
		::SetFocus(m_hWindow);


		IE_LOG(Verbose, TEXT("Window Initialized"));
		return true;
	}

	void Win32Window::PostInit()
	{
		RECT ClientRect = {};
		::GetClientRect(m_hWindow, &ClientRect);
		WindowResizeEvent Event(ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top, false);
		GetEventCallbackFn()(Event);
	}

	void Win32Window::RegisterWindowClass()
	{
		WNDCLASSEX wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);

		wc.style			= 0;
		wc.lpfnWndProc		= WindowProcedure;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= m_WindowsAppInstance;
		wc.hIcon			= m_Icon;
		wc.hCursor			= m_Cursor;
		wc.lpszMenuName		= m_MenuBarName;
		wc.hbrBackground	= 0;
		wc.lpszClassName	= m_WindowClassName.c_str();

		::RegisterClassEx(&wc);
		DWORD error = ::GetLastError();
		if (error > 0)
		{
			IE_LOG(Error, TEXT("An error occured while registering window class: %s"), StringHelper::WideToString(m_WindowClassName).c_str());
			IE_LOG(Error, TEXT("    Error: %s"), Platform::GetLastPlatformError());
		}
	}

	void Win32Window::OnWindowModeChanged()
	{
		switch (GetWindowMode())
		{
		case EWindowMode::WM_FullScreen:
			{
				//
				// Bring us into fullscreen mode.
				//
				
				// Save the old window rect so we can restore it when exiting fullscreen mode.
				::GetWindowRect(m_hWindow, &m_WindowRect);

				// Make the window borderless so that the client area can fill the screen.
				SetWindowLong(m_hWindow, GWL_STYLE, m_WindowStyle & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

				// Get the settings of the primary display
				DEVMODE DevMode = {};
				DevMode.dmSize = sizeof(DEVMODE);
				EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &DevMode);

				RECT FullscreenWindowRect= {
					DevMode.dmPosition.x,
					DevMode.dmPosition.y,
					DevMode.dmPosition.x + static_cast<LONG>(DevMode.dmPelsWidth),
					DevMode.dmPosition.y + static_cast<LONG>(DevMode.dmPelsHeight)
				};

				SetWindowPos(
					m_hWindow,
					HWND_TOPMOST,
					FullscreenWindowRect.left,
					FullscreenWindowRect.top,
					FullscreenWindowRect.right,
					FullscreenWindowRect.bottom,
					SWP_FRAMECHANGED | SWP_NOACTIVATE);

				ShowWindow(m_hWindow, SW_MAXIMIZE);

				IE_LOG(Log, TEXT("Entering fullscreen mode."));
			}
			break;
		case EWindowMode::WM_Windowed:
		{
			::SetWindowLong(m_hWindow, GWL_STYLE, m_WindowStyle);

			::SetWindowPos(
				m_hWindow,
				HWND_NOTOPMOST,
				m_WindowRect.left,
				m_WindowRect.top,
				m_WindowRect.right - m_WindowRect.left,
				m_WindowRect.bottom - m_WindowRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			::ShowWindow(m_hWindow, SW_NORMAL);

			IE_LOG(Log, TEXT("Exiting fullscreen mode."));
		}
			break;
		}
	}

	bool Win32Window::ProccessWindowMessages()
	{
		MSG msg;
		::ZeroMemory(&msg, sizeof(MSG));

		while (::PeekMessage(&msg,	// Where to store message (if one exists).
			m_hWindow,				// Handle to window we are checking messages for.
			0,						// Minimum Filter Msg Value - We are not filtering for specific messages but min and max could be used to do so.
			0,						// Maximum Filter Msg Value.
			PM_REMOVE))				// Post message read behavior, just remove the message once finished with it.
		{
			if (!TranslateAccelerator(msg.hwnd, m_hAccelerationTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		IE_ASSERT(_CrtCheckMemory(), "Heap is currupted!");
		return true;
	}

	Win32Window::~Win32Window()
	{
		IE_LOG(Warning, TEXT("Destroying window: %s"), StringHelper::WideToString(m_WindowTitle).c_str());
		Shutdown();
	}

	void Win32Window::OnUpdate()
	{
		ProccessWindowMessages();
	}

	bool Win32Window::SetWindowTitle(const EString& NewText, bool CompletlyOverride)
	{
		BOOL succeeded = true;
		if (CompletlyOverride) {
			succeeded = SetWindowText(m_hWindow, NewText.c_str());
		}
		else {
			m_WindowTitle = m_WindowTitle + L" - " + NewText;
			succeeded = SetWindowText(m_hWindow, m_WindowTitle.c_str());
		}
		return succeeded;
	}

	void* Win32Window::GetNativeWindow() const
	{
		return m_hWindow;
	}

	void Win32Window::Shutdown()
	{
		::CoUninitialize();

		if (m_hWindow != NULL)
		{
			::UnregisterClass(this->m_WindowClassName.c_str(), m_WindowsAppInstance);
			::DestroyWindow(m_hWindow);
		}
	}

}

#endif // IE_PLATFORM_BUILD_WIN32