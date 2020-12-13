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
	auto pApp = Insight::CreateApplication();

	try 
	{
		Insight::Win32WindowDescription WindowDesc(hInstance, IE_BIND_EVENT_FN(Insight::Application::OnEvent, pApp.get()), nCmdShow, lpCmdLine);
		std::shared_ptr<Insight::Win32Window> pWindowsWindow = std::make_shared<Insight::Win32Window>(WindowDesc);

		pApp->SetWindow(pWindowsWindow);
		pApp->Initialize();
	}
	catch (Insight::ieException& Ex) 
	{
		IE_DEBUG_LOG(LogSeverity::Critical, Ex.What());
		return Insight::Application::ieErrorCode_Failed;
	}

	return pApp->Run();
}