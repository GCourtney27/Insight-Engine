// Copyright Insight Interactive. All Rights Reserved.
#include <Engine_pch.h>

#include "Application.h"

#include "Insight/Runtime/AActor.h"
#include "Insight/Core/Layer/ImGui_Layer.h"
#include "Insight/Core/ie_Exception.h"
#include "Insight/Rendering/Renderer.h"

#if defined IE_PLATFORM_WINDOWS
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
	}

	Application::~Application()
	{
	}

	bool Application::InitializeCoreApplication()
	{
		// Create the game layer that will hsot all game logic.
		m_pGameLayer = new GameLayer();

		// Load the Scene
		std::string DocumentPath(FileSystem::GetProjectDirectory());
		DocumentPath += "/Assets/Scenes/";
		DocumentPath += TargetSceneName;
		if (!m_pGameLayer->LoadScene(DocumentPath)) {
			throw ieException("Failed to initialize scene");
		}
		Renderer::SetActiveCamera(&m_pGameLayer->GetScene()->GetSceneCamera());

		// Push core app layers to the layer stack
		PushCoreLayers();

		return true;
	}

	void Application::PostInit()
	{
		ResourceManager::Get().PostAppInit();
		IE_CORE_TRACE("Application Initialized");

		m_pGameLayer->PostInit();

	}

	void Application::Run(float DeltaMs)
	{
		// Update game logic.
		m_pGameLayer->Update(DeltaMs);

		// Update the layer stack.
		for (Layer* layer : m_LayerStack) 
			layer->OnUpdate(DeltaMs);

	}

	void Application::Shutdown()
	{
	}

	void Application::PushCoreLayers()
	{
		switch (Renderer::GetAPI())
		{
#if defined(IE_PLATFORM_WINDOWS)
		case Renderer::eTargetRenderAPI::D3D_11:
			IE_STRIP_FOR_GAME_DIST(m_pImGuiLayer = new D3D11ImGuiLayer());
			break;
		case Renderer::eTargetRenderAPI::D3D_12:
			IE_STRIP_FOR_GAME_DIST(m_pImGuiLayer = new D3D12ImGuiLayer());
			break;
#endif
		default:
			IE_CORE_ERROR("Failed to create ImGui layer in application with API of type \"{0}\"", Renderer::GetAPI());
			break;
		}

		IE_STRIP_FOR_GAME_DIST(PushOverlay(m_pImGuiLayer);)
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
		Dispatcher.Dispatch<SceneSaveEvent>(IE_BIND_EVENT_FN(Application::SaveScene));
		Dispatcher.Dispatch<AppBeginPlayEvent>(IE_BIND_EVENT_FN(Application::BeginPlay));
		Dispatcher.Dispatch<AppEndPlayEvent>(IE_BIND_EVENT_FN(Application::EndPlay));
		Dispatcher.Dispatch<AppScriptReloadEvent>(IE_BIND_EVENT_FN(Application::ReloadScripts));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			if (e.Handled()) break;
			(*--it)->OnEvent(e);
		}
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
		IE_CORE_INFO("Reloading C# Scripts");
		ResourceManager::Get().GetMonoScriptManager().ReCompile();
		return true;
	}


}
