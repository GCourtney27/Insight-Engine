#pragma once
#include "RenderWindow.h"
//#include "Input\\Keyboard\\KeyboardClass.h"
//#include "input\\Mouse\\MouseClass.h"
#include "..\\Input\\InputManager.h"
#include "Graphics/Graphics.h"

class WindowContainer
{
public:
	WindowContainer();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	RenderWindow render_window;
	Graphics gfx;

	InputManager input;

	KeyboardClass keyboard;
	MouseClass mouse;
	
};