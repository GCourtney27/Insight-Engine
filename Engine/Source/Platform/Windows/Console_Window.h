#pragma once

#include <Retina/Core.h>
#include <Windows.h>

namespace Retina {

	class RETINA_API ConsoleWindow
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

