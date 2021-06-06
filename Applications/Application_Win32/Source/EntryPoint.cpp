/*
	Entry point for Win32 desktop applications.
*/
#include "ClientApp.h"

#define MAX_STRING_LOAD_LENGTH 100

// App globals.
HINSTANCE g_WindowsAppInstance;
std::shared_ptr<Insight::Win32Window> g_pWindow;
std::unique_ptr<Insight::Engine> g_pEngine;

// Callbacks
void LoadWindowProps(Insight::Win32WindowDescription& WindowDesc);
LRESULT CALLBACK ProcessAccelCommand(const int& Command);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/*
	App driver.
*/
IE_ENTRY_POINT
{
	using namespace Insight;

	UNREFERENCED_PARAMETER(hPrevInstance);

	g_WindowsAppInstance = hInstance;
	
	g_pEngine = Insight::CreateEngine();

#if _DEBUG
	try 
#endif // _DEBUG
	{
		Win32WindowDescription WindowDesc(g_WindowsAppInstance, IE_BIND_EVENT_FN(Engine::OnEvent, g_pEngine.get()), nCmdShow, lpCmdLine);
		LoadWindowProps(WindowDesc);
		g_pWindow = std::make_shared<Win32Window>(WindowDesc);

		FString Title = TEXT("Insight Engine - "
			IE_PLATFORM_STRING 
			" - "
			IE_CONFIG_STRING);
		g_pWindow->SetWindowTitle(Title, true);
		
		g_pEngine->SetWindow(g_pWindow);
		g_pEngine->Initialize();
	}
#if _DEBUG
	catch (Insight::ieException& Ex)
	{
		// A fatal error has occured. Dump the application to a file.
		Debug::Logger::AppendMessageForCoreDump(Ex.What());
		Debug::Logger::InitiateCoreDump();
		return EC_Failed;
	}
#endif // _DEBUG

	return g_pEngine->Run();
}

void LoadWindowProps(Insight::Win32WindowDescription& WindowDesc)
{
	WCHAR LoadBuffer[MAX_STRING_LOAD_LENGTH];

	// Load the app title.
	LoadStringW(g_WindowsAppInstance, IDS_APP_TITLE, LoadBuffer, MAX_STRING_LOAD_LENGTH);
	WindowDesc.Title	= { LoadBuffer };
	// Load the app class name.
	LoadStringW(g_WindowsAppInstance, IDC_WIN32APP, LoadBuffer, MAX_STRING_LOAD_LENGTH);
	WindowDesc.Class	= { LoadBuffer };
	// Load the accelerator table.
	WindowDesc.AccelerationTable	= LoadAccelerators(g_WindowsAppInstance, MAKEINTRESOURCE(IDC_WIN32APP));
	// Load the menu bar via the string table name.
	WindowDesc.MenuBarName			= MAKEINTRESOURCEW(IDC_WIN32APP);
	// Assign the custom callback to proccess events from the accelerator table.
	WindowDesc.UserAccelCallback = Insight::Win32Window::MakeAccelCallback(ProcessAccelCommand);
	// Load the icon for the window.
	WindowDesc.Icon					= LoadIcon(g_WindowsAppInstance, MAKEINTRESOURCE(IDI_WIN32APP));
	// Load the cursor for the application.
	WindowDesc.Cursor				= ::LoadCursor(0, IDC_ARROW);
}

LRESULT CALLBACK ProcessAccelCommand(const int& Command)
{
	switch (Command)
	{
	case IDM_TOGGLEUI:
	{
		static bool EditorUIEnabled = true;
		EditorUIEnabled = !EditorUIEnabled;
		g_pEngine->GetEditorLayer().SetUIEnabled(EditorUIEnabled);
		break;
	}
	case (IDM_RENDERING_RELOADSHADERS):
	{
		Insight::ShaderReloadEvent event;
		g_pWindow->GetEventCallbackFn()(event);
		break;
	}
	/* Editor Submenu */
	case (IDM_EDITOR_BEGINPLAY):
	{
		Insight::AppBeginPlayEvent event;
		g_pWindow->GetEventCallbackFn()(event);
		break;
	}
	case (IDM_EDITOR_ENDPLAY):
	{
		Insight::AppEndPlayEvent event;
		g_pWindow->GetEventCallbackFn()(event);
		break;
	}
	/* File Submenu */
	case (IDM_FILE_ABOUT):
	{
		wchar_t AboutMsgBuffer[256];
		int APIVersion = ((int)Insight::Renderer::GetAPI()) + 10;
		const wchar_t* RTEnabled = Insight::Renderer::GetIsRayTraceEnabled() ? L"Enabled" : L"Disabled";
		swprintf_s(AboutMsgBuffer, L"Version - 1.8 \nRenderer - Direct3D %i (Ray Tracing: %s) \n\nVendor Runtime: \nMono - v6.8.0.123 \nAssimp - v3.3.1 \nRapidJson - v1.0.0 \nImGui - v1.75", APIVersion, RTEnabled);
		Platform::CreateMessageBox(AboutMsgBuffer, L"About Retina Editor", NULL);
		break;
	}
	case (IDM_FILE_SAVE):
	{

		break;
	}
	case (IDM_FILE_OPEN):
	{
		break;
	}
	case (IDM_FILE_QUIT):
	{
		DestroyWindow(static_cast<HWND>(g_pWindow->GetNativeWindow()));
		break;
	}
	default:
		break;
	}

	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
