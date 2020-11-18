/*
	Entry point for Win32 desktop applications.
*/
#include "Win32_Client_App.h"


int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow
)
{

	auto pApplication = Insight::CreateApplication();
	try 
	{
		std::shared_ptr<Insight::Win32Window> pWindowsWindow = std::make_shared<Insight::Win32Window>(Insight::WindowDescription());
		if (!pWindowsWindow->Init(hInstance, nCmdShow, lpCmdLine)) 
		{
			IE_FATAL_ERROR(L"Fatal Error: Failed to initialize window.");
			return false;
		}
		pWindowsWindow->SetEventCallback(IE_BIND_EVENT_FN(Insight::Application::OnEvent, pApplication.get()));
		pApplication->SetWindow(pWindowsWindow);

		if (!pApplication->Init()) 
		{
			IE_FATAL_ERROR(L"Failed to initialize core engine. Exiting.");
			return Insight::Application::ieErrorCode_Failed;
		}
	}
	catch (Insight::ieException& e) 
	{
		IE_DEBUG_LOG(LogSeverity::Critical, e.What());
		return Insight::Application::ieErrorCode_Failed;
	}
	return pApplication->Run();
}