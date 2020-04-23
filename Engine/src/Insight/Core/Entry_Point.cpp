#include "ie_pch.h"

#include "ClientApp.h"
#import "C:/VSDev/ComputerGraphics/Insight/Editor/Editor/bin/Debug/Editor.tlb" no_namespace
#include <thread>
// Copyright 2020 Garrett Courtney

/*=====================================================================

	Here, an entry point is decided for the application to start.
	*Note: If you decide to compile and run on mac, you must define IE_PLATFORM_MAC
	in premake file as well as change 'kind' under project "Application" to 'ConsoleApp'
	becasue mac does not recognize wWinMain as a valid entry point

 ======================================================================*/

extern Insight::Application* Insight::CreateApplication();

#if defined IE_PLATFORM_WINDOWS

void OpenDialog()
{
	CoInitialize(NULL);
	IWindowPtr obj;
	obj.CreateInstance(__uuidof(Window));
	obj->Tite("Hello World!");
	obj->Show();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	if(!Insight::Log::Init())
		IE_CORE_FATAL(L"Failed to Core logger.");
	IE_CORE_TRACE("Logger Initialized");

	auto app = Insight::CreateApplication();

	std::thread uiThread(OpenDialog);

	if (!app->InitializeAppForWindows(hInstance, nCmdShow))
	{
		IE_CORE_FATAL(L"Failed to initialize core engine. Exiting.");
		return 0;
	}

	app->Run();
	app->Shutdown();

	uiThread.join();
	delete app;

	CoUninitialize();
	Insight::Log::HoldForUserInput();
	return 0;
}
#elif IE_PLATFORM_MAC
int main(int argc, char** argv)
{
	auto app = Insight::CreateApplication();
	app->Init();
	app->Run();
	delete app;
	return 0;
}
#else
#error No valid entry point found for engine to begin execution.
#endif

