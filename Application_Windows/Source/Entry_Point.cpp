#include <Engine_pch.h>

#include "Client_App.h"

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow
)
{
	IE_STRIP_FOR_GAME_DIST(if (!Insight::Debug::Logger::Init())) {
		IE_FATAL_ERROR(L"Failed to create core logger.");
	}

	auto App = Insight::CreateApplication();

	{
		ScopedPerfTimer("Core application initialization", OutputType_Millis);

		try 
		{

			if (!App->InitializeAppForWindows(hInstance, nCmdShow)) 
			{
				IE_FATAL_ERROR(L"Failed to initialize core engine. Exiting.");
				return -1;
			}
		}
		catch (Insight::ieException& e) {
			IE_DEBUG_LOG(LogSeverity::Log, e.What());
		}
		App->PostInit();
	}

	App->Run();
	App->Shutdown();

	delete App;
	return 0;
}