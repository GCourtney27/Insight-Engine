#include "ie_pch.h"

#include "Windows_Window.h"
#include "Insight/Application.h"
#include "Insight/Events/ApplicationEvent.h"

namespace Insight {


	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	Insight::WindowsWindow::WindowsWindow(const WindowProps & props)
	{
		m_Data.WindowTitle = props.Title.c_str();
		m_Data.WindowClassName = "Insight Engine Class";
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

#ifdef IE_DEBUG
		CreateConsoleWindow(500, 120, 32, 120);
		printf("Hello From Application!");
#endif

		//Init(props);
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg) {
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDc = BeginPaint(hWnd, &ps);

			FillRect(hDc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

			EndPaint(hWnd, &ps);

			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			WindowCloseEvent event;

			return 0;
		}
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	void WindowsWindow::Init(const WindowProps & props)
	{
		WNDCLASSEXA wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = 0;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = *m_WindowsApplicationInstance;
		wc.hIcon = LoadIcon(0, IDI_WINLOGO);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.lpszMenuName = 0;
		wc.hbrBackground = 0;
		wc.lpszClassName = m_Data.WindowClassName;
		
		RegisterClassExA(&wc);
		int error = GetLastError();

		m_WindowHandle = CreateWindowExA(
			0,							   // Window Styles
			m_Data.WindowClassName,		   // Window Class
			m_Data.WindowTitle,			   // Window Title
			WS_OVERLAPPEDWINDOW,		   // Window Style

			0,							   // Start X
			0,							   // Start Y
			m_Data.Width,				   // Width
			m_Data.Height,				   // Height

			NULL,						   // Parent window
			NULL,						   // Menu
			*m_WindowsApplicationInstance, // Current Windows program application instance passed from WinMain
			NULL						   // Additional application data
		);

		//IE_CORE_ASSERT(m_WindowHandle != NULL);

		ShowWindow(m_WindowHandle, m_nCmdShowArgs);
	}

	bool WindowsWindow::ProccessWindowMessages()
	{

		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		while (PeekMessage(&msg,  // Where to store message (ifone exists)
			m_WindowHandle, //Handle to window we are checking messages for
			0, // Minimum Filter Msg Value - We are not filterinf for specific messages but min and max could be used to do so
			0, // Maximum Filter Msg Value
			PM_REMOVE)) // Remove mesage after captureing it via PeekMessage
		{
			if (msg.message == WM_QUIT)
				return false;
			TranslateMessage(&msg);  // Translate message from virtual key message into character messages
			DispatchMessage(&msg); // Dispatch message to our Window Proc for this window
		}

		// Check if the window was closed with the top right X button
		if (msg.message == WM_NULL)
		{
			if (!IsWindow(m_WindowHandle))
			{
				m_WindowHandle = NULL; // Message proccessing loop takes care of destroying this window
				UnregisterClassA(m_Data.WindowClassName, *m_WindowsApplicationInstance);
				return false;
			}
		}
#ifdef _DEBUG
		//assert(_CrtCheckMemory()); // Make sure the heap isn's corrupted on exit
#endif

		return true;
	}

	void WindowsWindow::CreateConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns)
	{
		// Our temp console info struct
		CONSOLE_SCREEN_BUFFER_INFO coninfo;

		// Get the console info and set the number of lines
		AllocConsole();
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
		coninfo.dwSize.Y = bufferLines;
		coninfo.dwSize.X = bufferColumns;
		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

		SMALL_RECT rect;
		rect.Left = 0;
		rect.Top = 0;
		rect.Right = windowColumns;
		rect.Bottom = windowLines;
		SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &rect);

		FILE *stream;
		freopen_s(&stream, "CONIN$", "r", stdin);
		freopen_s(&stream, "CONOUT$", "w", stdout);
		freopen_s(&stream, "CONOUT$", "w", stderr);

		// Prevent accidental console window close
		m_ConsoleWindowHandle = GetConsoleWindow();
		m_ConsoleWindowHMenu = GetSystemMenu(m_ConsoleWindowHandle, FALSE);
		EnableMenuItem(m_ConsoleWindowHMenu, SC_CLOSE, MF_GRAYED);
	}

	WindowsWindow::~WindowsWindow()
	{
	}

	void WindowsWindow::OnUpdate()
	{
	}

	void WindowsWindow::SetEventCallback(const EventCallbackFn & callback)
	{
	}

	void * WindowsWindow::GetNativeWindow() const
	{
		return nullptr;
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
	}

	bool WindowsWindow::IsVsyncActive() const
	{
		return false;
	}
	
	void WindowsWindow::Shutdown()
	{
	}

}

