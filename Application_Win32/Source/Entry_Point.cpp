/*
	Entry point for Win32 desktop applications.
*/
#include "Win32_Client_App.h"

#define MAX_STRING_LOAD_LENGTH 100
HINSTANCE g_AppInstance;

std::shared_ptr<Insight::Win32Window> g_WindowsWindow;

void LoadWindowProps(Insight::Win32WindowDescription& WindowDesc);
LRESULT CALLBACK CustomParamParse(const int& Command);

int APIENTRY wWinMain(
	_In_		HINSTANCE hInstance,
	_In_opt_	HINSTANCE hPrevInstance,
	_In_		LPWSTR lpCmdLine,
	_In_		int nCmdShow
)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	g_AppInstance = hInstance;
	
	auto pApp = Insight::CreateApplication();
	try 
	{
		Insight::Win32WindowDescription WindowDesc(g_AppInstance, IE_BIND_EVENT_FN(Insight::Application::OnEvent, pApp.get()), nCmdShow, lpCmdLine);
		LoadWindowProps(WindowDesc);
		g_WindowsWindow = std::make_shared<Insight::Win32Window>(WindowDesc);

		pApp->SetWindow(g_WindowsWindow);
		pApp->Initialize();
	}
	catch (Insight::ieException& Ex) 
	{
		IE_DEBUG_LOG(LogSeverity::Critical, Ex.What());
		return Insight::Application::ieErrorCode_Failed;
	}

	return pApp->Run();
}

void LoadWindowProps(Insight::Win32WindowDescription& WindowDesc)
{
	WCHAR LoadBuffer[MAX_STRING_LOAD_LENGTH];

	// Load the app title.
	LoadStringW(g_AppInstance, IDS_APP_TITLE, LoadBuffer, MAX_STRING_LOAD_LENGTH);
	WindowDesc.Title = { LoadBuffer };
	// Load the app class name.
	LoadStringW(g_AppInstance, IDC_WIN32APP, LoadBuffer, MAX_STRING_LOAD_LENGTH);
	WindowDesc.Class = { LoadBuffer };
	WindowDesc.AccelerationTable = LoadAccelerators(g_AppInstance, MAKEINTRESOURCE(IDC_WIN32APP));
	//WindowDesc.MenuBarName = MAKEINTRESOURCEW(IDC_WIN32APP);
	WindowDesc.CustomCallback = Insight::Win32Window::MakeCustomCallback(CustomParamParse);
	
}

LRESULT CALLBACK CustomParamParse(const int& Command)
{
	switch (Command)
	{
	case IDM_TOGGLEUI:
	{
		OutputDebugStringW(L"Key pressed.");
	}
	default:
		return 0;
	}
	return 0;
}
