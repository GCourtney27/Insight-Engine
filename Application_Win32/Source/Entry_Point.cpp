/*
	Entry point for Win32 desktop applications.
*/
#include "Win32_Client_App.h"

#define MAX_STRING_LOAD_LENGTH 100

// App globals.
HINSTANCE g_AppInstance;
std::shared_ptr<Insight::Win32Window> g_pWindow;
std::unique_ptr<Insight::Application> g_pApp;

// Callbacks
void LoadWindowProps(Insight::Win32WindowDescription& WindowDesc);
LRESULT CALLBACK ProcessAccelCommand(const int& Command);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/*
	App driver.
*/
int APIENTRY wWinMain(
	_In_		HINSTANCE hInstance,
	_In_opt_	HINSTANCE hPrevInstance,
	_In_		LPWSTR lpCmdLine,
	_In_		int nCmdShow
)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	g_AppInstance = hInstance;
	
	g_pApp = Insight::CreateApplication();
	try 
	{
		Insight::Win32WindowDescription WindowDesc(g_AppInstance, IE_BIND_EVENT_FN(Insight::Application::OnEvent, g_pApp.get()), nCmdShow, lpCmdLine);
		LoadWindowProps(WindowDesc);
		g_pWindow = std::make_shared<Insight::Win32Window>(WindowDesc);

		g_pApp->SetWindow(g_pWindow);
		g_pApp->Initialize();
	}
	catch (Insight::ieException& Ex) 
	{
		IE_DEBUG_LOG(LogSeverity::Critical, Ex.What());
		return Insight::Application::ieErrorCode_Failed;
	}

	return g_pApp->Run();
}

void LoadWindowProps(Insight::Win32WindowDescription& WindowDesc)
{
	WCHAR LoadBuffer[MAX_STRING_LOAD_LENGTH];

	// Load the app title.
	LoadStringW(g_AppInstance, IDS_APP_TITLE, LoadBuffer, MAX_STRING_LOAD_LENGTH);
	WindowDesc.Title	= { LoadBuffer };
	// Load the app class name.
	LoadStringW(g_AppInstance, IDC_WIN32APP, LoadBuffer, MAX_STRING_LOAD_LENGTH);
	WindowDesc.Class	= { LoadBuffer };
	// Load the accelerator table.
	WindowDesc.AccelerationTable	= LoadAccelerators(g_AppInstance, MAKEINTRESOURCE(IDC_WIN32APP));
	// Load the menu bar via the string table name.
	WindowDesc.MenuBarName			= MAKEINTRESOURCEW(IDC_WIN32APP);
	// Assign the custom callback to proccess events from the accelerator table.
	WindowDesc.UserAccelCallback = Insight::Win32Window::MakeAccelCallback(ProcessAccelCommand);
	// Load the icon for the window.
	WindowDesc.Icon					= LoadIcon(g_AppInstance, MAKEINTRESOURCE(IDI_WIN32APP));
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
		g_pApp->GetEditorLayer().SetUIEnabled(EditorUIEnabled);
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
		/*wchar_t AboutMsgBuffer[256];
		int APIVersion = ((int)Insight::Renderer::GetAPI()) + 10;
		const wchar_t* RTEnabled = Insight::Renderer::GetIsRayTraceEnabled() ? L"Enabled" : L"Disabled";
		swprintf_s(AboutMsgBuffer, L"Version - 1.8 \nRenderer - Direct3D %i (Ray Tracing: %s) \n\nVendor Runtime: \nMono - v6.8.0.123 \nAssimp - v3.3.1 \nRapidJson - v1.0.0 \nImGui - v1.75", APIVersion, RTEnabled);
		g_pWindow->CreateMessageBox(AboutMsgBuffer, L"About Retina Editor");*/
		DialogBox(g_AppInstance, MAKEINTRESOURCE(IDD_ABOUT), static_cast<HWND>(g_pWindow->GetNativeWindow()), About);
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
