/*
	Entry point for windows applications.
*/
#include "Client_App.h"


int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow
)
{
	IE_STRIP_FOR_GAME_DIST(
		if (!Insight::Debug::Logger::Init()) 
		{
			IE_FATAL_ERROR(L"Failed to create core logger.");
		}
	)

	auto pApplication = Insight::CreateApplication();

	{
		ScopedPerfTimer("Core application initialization", OutputType_Millis);

		try 
		{
			std::shared_ptr<Insight::Win32Window> pWindowsWindow = std::make_shared<Insight::Win32Window>(Insight::WindowProps());
			if (!pWindowsWindow->Init(hInstance, nCmdShow, lpCmdLine)) 
			{
				IE_FATAL_ERROR(L"Fatal Error: Failed to initialize window.");
				return false;
			}
			pWindowsWindow->SetEventCallback(std::bind(&Insight::Application::OnEvent, pApplication, std::placeholders::_1));
			pApplication->SetWindow(pWindowsWindow);

			if (!pApplication->Init()) 
			{
				IE_FATAL_ERROR(L"Failed to initialize core engine. Exiting.");
				return -1;
			}
		}
		catch (Insight::ieException& e) 
		{
			IE_DEBUG_LOG(LogSeverity::Critical, e.What());
			if (pApplication != nullptr) delete pApplication;
			return -1;
		}
		pApplication->PostInit();
	}

	pApplication->Run();

	delete pApplication;

	return 0;
}