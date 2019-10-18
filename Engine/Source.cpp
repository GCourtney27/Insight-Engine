#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,	_In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to call CoInitialize.");
		return -1;
	}

	Engine engine;
	if (engine.Initialize(hInstance, "Insight Engine", "EngineWindowClass", 1920, 1000))
	{
		while (engine.ProccessMessages() == true)
		{
			engine.Update();
			engine.RenderFrame();
		}
	}
	engine.Shutdown();

	return 0;
}