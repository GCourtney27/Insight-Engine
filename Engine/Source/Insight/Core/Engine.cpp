#include <Engine_pch.h>

#include "Engine.h"

#if defined IE_PLATFORM_WINDOWS
#include "Platform/Windows/Windows_Window.h"
#endif

namespace Insight {


	static const char* ProjectName = "Development-Project";
	static const char* TargetSceneName = "Debug.iescene";

	Engine::Engine()
	{
		m_pWindow = std::unique_ptr<Window>(Window::Create());
	}

	Engine::~Engine()
	{
		Renderer::Destroy();
	}

	int Engine::RunWin32App(Application* pApp, HINSTANCE hInstance, LPWSTR CmdArgs, int NumCmdArgs)
	{
		m_pApplication = pApp;

		// Initialize the logger
		IE_STRIP_FOR_GAME_DIST(
			if (!Insight::Log::Init()) {
				IE_CORE_FATAL(L"Failed to Core logger.");
			}
			IE_CORE_TRACE("Logger Initialized");
		);

		m_pWindow->SetEventCallback(IE_BIND_EVENT_FN(Engine::OnEvent));

		WindowsWindow* pWindow = (WindowsWindow*)(m_pWindow.get());
		pWindow->SetWindowsSessionProps(hInstance, NumCmdArgs);
		if (!pWindow->Init(WindowProps())) 
			IE_CORE_FATAL(L"Fatal Error: Failed to initialize window.");

		// Initize the main file system
		FileSystem::Init(ProjectName);

		// Create and initialize the renderer
		Renderer::SetSettingsAndCreateContext(FileSystem::LoadGraphicsSettingsFromJson(), m_pWindow.get());
		Renderer::Init();

		pApp->InitializeCoreApplication();

		Renderer::PostInit();
		m_pWindow->PostInit();
		pApp->PostInit();

		// Setup Threads
		std::thread RenderThread(&Engine::RenderThread, this);

		GameThread();

		RenderThread.join();
		pApp->Shutdown();

		// TODO
		//return pApp->GetLastErrorCode();
		return 0;
	}

	int Engine::RenderThread()
	{
		// Cache the layers for the UI render pass.
		GameLayer& GameLayerRef = m_pApplication->GetGameLayer();
		LayerStack& LayerStackRef = m_pApplication->GetLayerStack();
		ImGuiLayer& ImGuiLayerRef = m_pApplication->GetImGuiLayer();

		while (m_Running)
		{
			m_GPUThreadTimer.Tick();
			
			Renderer::OnUpdate(m_GPUThreadTimer.DeltaTime());

			// Prepare for rendering.
			Renderer::OnPreFrameRender();

			// Render the world.
			Renderer::OnRender();

			// Render the Editor/UI last.
			IE_STRIP_FOR_GAME_DIST
			(
				ImGuiLayerRef.Begin();
				for (Layer* pLayer : LayerStackRef)
					pLayer->OnImGuiRender();
				GameLayerRef.OnImGuiRender();
				ImGuiLayerRef.End();
			);

			// Submit for draw and present.
			Renderer::ExecuteDraw();
			Renderer::SwapBuffers();
		}
		return 0;
	}

	int Engine::GameThread()
	{
		IE_ADD_FOR_GAME_DIST(
			m_pApplication->OnEvent(AppBeginPlayEvent{});
		);

		while (m_Running)
		{
			m_GameThreadTimer.Tick();
			
			m_pWindow->OnUpdate();
			m_InputDispatcher.UpdateInputs();

			m_pApplication->Run(m_GameThreadTimer.DeltaTime());
		}
		return 0;
	}



	// ---------------------
	//		Events			|
	// ---------------------

	void Engine::OnEvent(Event& e)
	{
		EventDispatcher Dispatcher(e);
		Dispatcher.Dispatch<WindowCloseEvent>(IE_BIND_EVENT_FN(Engine::OnWindowClose));
		Dispatcher.Dispatch<WindowResizeEvent>(IE_BIND_EVENT_FN(Engine::OnWindowResize));
		Dispatcher.Dispatch<WindowToggleFullScreenEvent>(IE_BIND_EVENT_FN(Engine::OnWindowFullScreen));
		Dispatcher.Dispatch<ShaderReloadEvent>(IE_BIND_EVENT_FN(Engine::ReloadShaders));

		m_InputDispatcher.ProcessInputEvent(e);

		m_pApplication->OnEvent(e);
	}

	bool Engine::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Engine::OnWindowResize(WindowResizeEvent& e)
	{
		m_pWindow->Resize(e.GetWidth(), e.GetHeight(), e.GetIsMinimized());
		Renderer::PushEvent<WindowResizeEvent>(e);
		return true;
	}

	bool Engine::OnWindowFullScreen(WindowToggleFullScreenEvent& e)
	{
		m_pWindow->ToggleFullScreen(e.GetFullScreenEnabled());
		Renderer::PushEvent<WindowToggleFullScreenEvent>(e);
		return true;
	}

	bool Engine::ReloadShaders(ShaderReloadEvent& e)
	{
		Renderer::PushEvent<ShaderReloadEvent>(e);
		return true;
	}

}