#include <Engine_pch.h>

// The contents can only be accessed on a Win32 platform.
// If it is not present than just compile it out.
#if defined (IE_PLATFORM_BUILD_WIN32)
#include "Console_Window.h"

namespace Insight {



	ConsoleWindow::ConsoleWindow()
	{
		Init();
	}

	ConsoleWindow::~ConsoleWindow()
	{
		Shutdown();
	}

	bool ConsoleWindow::Init(int bufferLines, int bufferColumns, int windowLines, int windowColumns)
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
		m_WindowHandle = GetConsoleWindow();
		m_WindowHMenu = GetSystemMenu(m_WindowHandle, FALSE);
		EnableMenuItem(m_WindowHMenu, SC_CLOSE, MF_GRAYED);

		return true;
	}

	void ConsoleWindow::Shutdown()
	{
		DestroyWindow(m_WindowHandle);
		DestroyMenu(m_WindowHMenu);
	}



}
#endif  // IE_PLATFORM_BUILD_WIN32
