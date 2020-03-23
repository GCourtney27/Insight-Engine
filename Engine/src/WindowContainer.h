#pragma once
#include "RenderWindow.h"
#include "..\\Input\\InputManager.h"

class WindowContainer
{
public:
	WindowContainer();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	RenderWindow render_window;
	
	InputManager input;
	
};