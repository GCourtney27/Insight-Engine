// Copyright Insight Interactive. All Rights Reserved.
#include <Engine_pch.h>

#include "Application.h"

#include "Insight/Runtime/AActor.h"
#include "Insight/Core/Layer/ImGui_Layer.h"
#include "Insight/Core/ie_Exception.h"
#include "Insight/Rendering/Renderer.h"

#if defined (IE_PLATFORM_WINDOWS)
	#include "Platform/DirectX_11/Wrappers/D3D11_ImGui_Layer.h"
	#include "Platform/DirectX_12/Wrappers/D3D12_ImGui_Layer.h"
	#include "Platform/Win32/Win32_Window.h"
#endif

// TODO: Make the project hot swapable
// TODO: Make sample projects
// Scenes (Development-Project)
// ----------------------------
// DemoScene
// MultipleLights
static const char* ProjectName = "Development-Project";
static const char* TargetSceneName = "Debug.iescene";

namespace Insight {

	
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		IE_ASSERT(!s_Instance, "Trying to create Application instance when one already exists!");
		s_Instance = this;


		// Initialize the core logger.
		IE_STRIP_FOR_GAME_DIST(
			if (!Insight::Debug::Logger::Init())
			{
				IE_FATAL_ERROR(L"Failed to create core logger.");
			}
		)
	}

	Application::~Application()
	{
	}

	void Application::Initialize()
	{
		ScopedPerfTimer("Core application initialization", OutputType_Millis);

		// Initize the main file system.
		FileSystem::Init(ProjectName);

		// Create and initialize the renderer.
		Renderer::SetSettingsAndCreateContext(FileSystem::LoadGraphicsSettingsFromJson(), m_pWindow.get());

		// Create the game layer that will host all game logic.
		m_pGameLayer = new GameLayer();

		// Load the Scene
		std::string DocumentPath = FileSystem::GetRelativeContentDirectory("/Scenes/");
		DocumentPath += TargetSceneName;
		if (!m_pGameLayer->LoadScene(DocumentPath)) {
			throw ieException("Failed to initialize scene");
		}
		Renderer::SetActiveCamera(&m_pGameLayer->GetScene()->GetSceneCamera());

		// Push core app layers to the layer stack
		PushCoreLayers();
	}

	void Application::PostInit()
	{
		Renderer::PostInit();

		m_pWindow->PostInit();
		ResourceManager::Get().PostAppInit();
		m_pGameLayer->PostInit();

		IE_DEBUG_LOG(LogSeverity::Verbose, "Application Initialized");
	}

#define EDITOR_UI_ENABLED 0

	float g_GPUThreadFPS = 0.0f;
	void Application::RenderThread()
	{
		FrameTimer GraphicsTimer;

		while (m_Running)
		{
			GraphicsTimer.Tick();
			g_GPUThreadFPS = GraphicsTimer.FPS();

			Renderer::OnUpdate(GraphicsTimer.DeltaTime());

			// Prepare for rendering. 
			Renderer::OnPreFrameRender();

			// Render the world. 
			Renderer::OnRender();

			// Render the Editor/UI last. 
#if EDITOR_UI_ENABLED
			IE_STRIP_FOR_GAME_DIST
			(
			m_pImGuiLayer->Begin();
			for (Layer* pLayer : m_LayerStack)
				pLayer->OnImGuiRender();
			m_pGameLayer->OnImGuiRender();
			m_pImGuiLayer->End();
			);
#endif

			// Submit for draw and present. 
			Renderer::ExecuteDraw();
			Renderer::SwapBuffers();
		}
	}

	Application::ieErrorCode Application::Run()
	{
		IE_ADD_FOR_GAME_DIST(
			BeginPlay(AppBeginPlayEvent{})
		);

		// Put all rendering on another thread. 
		std::thread RenderThread(&Application::RenderThread, this);
		
		while (m_Running)
		{
			m_FrameTimer.Tick();
			float DeltaMs = m_FrameTimer.DeltaTime();
			m_pWindow->SetWindowTitleFPS(g_GPUThreadFPS);

			// Process the window's Messages 
			m_pWindow->OnUpdate();

			static float WorldSeconds = 0.0f;
			WorldSeconds += DeltaMs;
			pSCDemoBall->Translate(0.0f, std::sin(WorldSeconds) * 0.02f, 0.0f);
			

			// Update the input system. 
			m_InputDispatcher.UpdateInputs(DeltaMs);

			// Update game logic. 
			m_pGameLayer->Update(DeltaMs);

			// Update the layer stack. 
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(DeltaMs);
		}

		// Close the render thread and flush the GPU.
		RenderThread.join();

		// Shutdown the application and release all resources.
		Shutdown();

		return ieErrorCode_Success;
	}

	Application::ieErrorCode Application::RunSingleThreaded()
	{
		{
			m_FrameTimer.Tick();
			float DeltaMs = m_FrameTimer.DeltaTime();
			m_pWindow->SetWindowTitleFPS(g_GPUThreadFPS);

			// Process the window's Messages 
			m_pWindow->OnUpdate();

			static float WorldSeconds = 0.0f;
			WorldSeconds += DeltaMs;
			pSCDemoBall->Translate(0.0f, std::sin(WorldSeconds) * 0.02f, 0.0f);

			{
				static FrameTimer GraphicsTimer;
				GraphicsTimer.Tick();
				g_GPUThreadFPS = GraphicsTimer.FPS();

				Renderer::OnUpdate(GraphicsTimer.DeltaTime());

				// Prepare for rendering. 
				Renderer::OnPreFrameRender();

				// Render the world. 
				Renderer::OnRender();

				// Render the Editor/UI last. 
#if EDITOR_UI_ENABLED
				IE_STRIP_FOR_GAME_DIST
				(
					m_pImGuiLayer->Begin();
				for (Layer* pLayer : m_LayerStack)
					pLayer->OnImGuiRender();
				m_pGameLayer->OnImGuiRender();
				m_pImGuiLayer->End();
				);
#endif

				// Submit for draw and present. 
				Renderer::ExecuteDraw();
				Renderer::SwapBuffers();
			}

			// Update the input system. 
			m_InputDispatcher.UpdateInputs(DeltaMs);

			// Update game logic. 
			m_pGameLayer->Update(DeltaMs);

			// Update the layer stack. 
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(DeltaMs);
		}
		return ieErrorCode_Success;
	}

	void Application::Shutdown()
	{
	}

	void Application::PushCoreLayers()
	{
		switch (Renderer::GetAPI())
		{
#if defined(IE_PLATFORM_WINDOWS)
		case Renderer::TargetRenderAPI::Direct3D_11:
			IE_STRIP_FOR_GAME_DIST(m_pImGuiLayer = new D3D11ImGuiLayer());
			break;
		case Renderer::TargetRenderAPI::Direct3D_12:
#if EDITOR_UI_ENABLED
			IE_STRIP_FOR_GAME_DIST(m_pImGuiLayer = new D3D12ImGuiLayer());
#endif
			break;
#endif
		default:
			IE_DEBUG_LOG(LogSeverity::Error, "Failed to create ImGui layer in application with API of type \"{0}\"", Renderer::GetAPI());
			break;
		}

#if EDITOR_UI_ENABLED
		IE_STRIP_FOR_GAME_DIST(PushOverlay(m_pImGuiLayer);)
#endif
		m_pEditorLayer = new EditorLayer();
		IE_STRIP_FOR_GAME_DIST(PushOverlay(m_pEditorLayer);)

		m_pPerfOverlay = new PerfOverlay();
		PushOverlay(m_pPerfOverlay);
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverLay(layer);
		layer->OnAttach();
	}



	// -----------------
	// Events Callbacks |
	// -----------------

	void Application::OnEvent(Event& e)
	{
		EventDispatcher Dispatcher(e);
		Dispatcher.Dispatch<WindowCloseEvent>(IE_BIND_LOCAL_EVENT_FN(Application::OnWindowClose));
		Dispatcher.Dispatch<WindowResizeEvent>(IE_BIND_LOCAL_EVENT_FN(Application::OnWindowResize));
		Dispatcher.Dispatch<WindowToggleFullScreenEvent>(IE_BIND_LOCAL_EVENT_FN(Application::OnWindowFullScreen));
		Dispatcher.Dispatch<SceneSaveEvent>(IE_BIND_LOCAL_EVENT_FN(Application::SaveScene));
		Dispatcher.Dispatch<AppBeginPlayEvent>(IE_BIND_LOCAL_EVENT_FN(Application::BeginPlay));
		Dispatcher.Dispatch<AppEndPlayEvent>(IE_BIND_LOCAL_EVENT_FN(Application::EndPlay));
		Dispatcher.Dispatch<AppScriptReloadEvent>(IE_BIND_LOCAL_EVENT_FN(Application::ReloadScripts));
		Dispatcher.Dispatch<ShaderReloadEvent>(IE_BIND_LOCAL_EVENT_FN(Application::ReloadShaders));

		// Process input event callbacks. 
		m_InputDispatcher.ProcessInputEvent(e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		m_pWindow->Resize(e.GetWidth(), e.GetHeight(), e.GetIsMinimized());
		Renderer::PushEvent<WindowResizeEvent>(e);

		return true;
	}

	bool Application::OnWindowFullScreen(WindowToggleFullScreenEvent& e)
	{
		m_pWindow->SetFullScreenEnabled(e.GetFullScreenEnabled());
		Renderer::PushEvent<WindowToggleFullScreenEvent>(e);
		return true;
	}

	bool Application::SaveScene(SceneSaveEvent& e)
	{
		std::future<bool> Future = std::async(std::launch::async, FileSystem::WriteSceneToJson, m_pGameLayer->GetScene());
		return true;
	}

	bool Application::BeginPlay(AppBeginPlayEvent& e)
	{
		PushLayer(m_pGameLayer);
		return true;
	}

	bool Application::EndPlay(AppEndPlayEvent& e)
	{
		m_pGameLayer->EndPlay();
		m_LayerStack.PopLayer(m_pGameLayer);
		m_pGameLayer->OnDetach();
		return true;
	}

	bool Application::ReloadScripts(AppScriptReloadEvent& e)
	{
		IE_DEBUG_LOG(LogSeverity::Log, "Reloading C# Scripts");
		ResourceManager::Get().GetMonoScriptManager().ReCompile();
		return true;
	}

	bool Application::ReloadShaders(ShaderReloadEvent& e)
	{
		//Renderer::OnShaderReload(); 
		Renderer::PushEvent<ShaderReloadEvent>(e);
		return true;
	}
}
