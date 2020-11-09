// Copyright 2020 Garrett Courtney

#include <Engine_pch.h>

#include "ClientApp.h"
#include "Insight/Core/ie_Exception.h"
#include "Insight/Utilities/Profiling.h"

/*=====================================================================

	Here, an entry point is decided for the application to start.
	*Note: If you decide to compile and run on mac, you must define IE_PLATFORM_MAC
	in premake file as well as change 'kind' under project "Application" to 'ConsoleApp'
	becasue mac does not recognize wWinMain as a valid entry point

 ======================================================================*/

extern Insight::Application* Insight::CreateApplication();


#if defined IE_PLATFORM_WINDOWS

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	IE_STRIP_FOR_GAME_DIST(if (!Insight::Debug::Logger::Init())) {
		IE_FATAL_ERROR(L"Failed to Core logger.");
	}


	auto App = Insight::CreateApplication();

	{
		ScopedPerfTimer("Core application initialization", OutputType_Millis);

		try {

			if (!App->InitializeAppForWindows(hInstance, nCmdShow)) {
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

	//Insight::Log::HoldForUserInput();
	return 0;
}

#elif IE_PLATFORM_UWP



#elif IE_PLATFORM_MAC

int main(int argc, char** argv)
{
	auto App = Insight::CreateApplication();
	App->Init();
	App->Run();
	delete App;
	return 0;
}

#else
#error No valid entry point found for engine to begin execution.
#endif

