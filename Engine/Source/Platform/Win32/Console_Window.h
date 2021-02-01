#pragma once

#include <Runtime/Core.h>
#if IE_PLATFORM_BUILD_WIN32
#include <Windows.h>
namespace Insight {

	class INSIGHT_API ConsoleWindow
	{
	public:
		ConsoleWindow();
		~ConsoleWindow();

	private:
		bool Init(int bufferLines = 700, int bufferColumns = 320, int windowLines = 42, int windowColumns = 170);
		void Shutdown();
	private:
		HWND m_WindowHandle;
		HMENU m_WindowHMenu;
	};

}

#endif