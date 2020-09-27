#include <Engine_pch.h>

#include "ClientApp.h"
#include "Retina/Core/rn_Exception.h"
#include "Retina/Utilities/Profiling.h"

// Copyright 2020 Garrett Courtney

/*=====================================================================

	Here, an entry point is decided for the application to start.
	*Note: If you decide to compile and run on mac, you must define RN_PLATFORM_MAC
	in premake file as well as change 'kind' under project "Application" to 'ConsoleApp'
	becasue mac does not recognize wWinMain as a valid entry point

 ======================================================================*/

extern Retina::Application* Retina::CreateApplication();

#if defined RN_PLATFORM_WINDOWS

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	RN_STRIP_FOR_GAME_DIST(if (!Retina::Log::Init())) {
		RN_CORE_FATAL(L"Failed to Core logger.");
	}
	RN_CORE_TRACE("Logger Initialized");

	auto App = Retina::CreateApplication();

	{
		ScopedPerfTimer("Core application initialization", eOutputType_Millis);

		try {

			if (!App->InitializeAppForWindows(hInstance, nCmdShow)) {
				RN_CORE_FATAL(L"Failed to initialize core engine. Exiting.");
				return -1;
			}
		}
		catch (Retina::ieException& e) {
			RN_CORE_INFO(e.What());
		}
		App->PostInit();
	}

	App->Run();
	App->Shutdown();

	delete App;

	//Retina::Log::HoldForUserInput();
	return 0;
}
#elif RN_PLATFORM_MAC
int main(int argc, char** argv)
{
	auto App = Retina::CreateApplication();
	App->Init();
	App->Run();
	delete App;
	return 0;
}
#else
#error No valid entry point found for engine to begin execution.
#endif

