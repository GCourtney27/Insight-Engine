#include <Engine_pch.h>

// The contents can only be accessed on a Win32 platform.
// If it is not present than just compile it out.

#if IE_PLATFORM_BUILD_WIN32
#include "ConsoleWindow.h"

namespace Insight {


	ConsoleWindow::ConsoleWindow()
		: m_WindowHandle(NULL)
		, m_OutputHandle(NULL)
		, m_WindowHMenu(NULL)
	{
	}

	ConsoleWindow::~ConsoleWindow()
	{
		Destroy();
	}

	bool ConsoleWindow::Create(const ConsoleWindowDesc& Desc)
	{
		m_Desc = Desc;

		// Our temp console info struct
		CONSOLE_SCREEN_BUFFER_INFO coninfo;

		// Get the console info and set the number of lines
		AllocConsole();
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
		coninfo.dwSize.Y = (SHORT)m_Desc.BufferDims.X;
		coninfo.dwSize.X = (SHORT)m_Desc.BufferDims.Y;
		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
		
		m_OutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		SMALL_RECT rect;
		rect.Left = 0;
		rect.Top = 0;
		rect.Right = (SHORT)m_Desc.WindowDims.X;
		rect.Bottom = (SHORT)m_Desc.WindowDims.Y;
		SetConsoleWindowInfo(m_OutputHandle, TRUE, &rect);

		FILE *stream;
		freopen_s(&stream, "CONIN$", "r", stdin);
		freopen_s(&stream, "CONOUT$", "w", stdout);
		freopen_s(&stream, "CONOUT$", "w", stderr);

		if (!m_Desc.CanClose)
		{
			// Prevent accidental console window close
			m_WindowHandle = GetConsoleWindow();
			m_WindowHMenu = GetSystemMenu(m_WindowHandle, FALSE);
			EnableMenuItem(m_WindowHMenu, SC_CLOSE, MF_GRAYED);
		}

		// Set the default console color.
		SetForegroundColor(m_Desc.DefaultForegroundColor);

		bool Valid = IsWindow(m_WindowHandle) && m_OutputHandle && m_WindowHMenu;
		return Valid;
	}

	void ConsoleWindow::Destroy()
	{
		DestroyWindow(m_WindowHandle);
		DestroyMenu(m_WindowHMenu);
		CloseHandle(m_OutputHandle);
	}
}
#endif  // IE_PLATFORM_BUILD_WIN32
