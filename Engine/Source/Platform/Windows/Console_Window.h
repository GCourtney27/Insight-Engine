#pragma once

#include <Insight/Core.h>
#include <Windows.h>

namespace Insight {

	class INSIGHT_API ConsoleWindow
	{
	public:
		ConsoleWindow();
		~ConsoleWindow();

	private:
		bool Init(int bufferLines = 500, int bufferColumns = 120, int windowLines = 32, int windowColumns = 120);
		void Shutdown();
	private:
		HWND m_WindowHandle;
		HMENU m_WindowHMenu;
	};

}

