#include <ie_pch.h>

#include "Application.h"

#include "Insight/Input/Input.h"
#include "Insight/Runtime/AActor.h"
#include "Insight/Layer_Types/ImGui_Layer.h"
#include "Platform/Windows/Windows_Window.h"
#include "Insight/Core/ieException.h"
#include "Insight/Rendering/Renderer.h"

// Scenes (Development-Project)
// ----------------------------
// DemoScene
// MultipleLights
static const char* ProjectName = "Development-Project";
static const char* TargetSceneName = "DemoScene.iescene";

namespace Insight {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		IE_ASSERT(!s_Instance, "Trying to create Application instance when one already exists!");
		s_Instance = this;

		IE_STRIP_FOR_GAME_DIST(m_pImGuiLayer = new ImGuiLayer());
		IE_STRIP_FOR_GAME_DIST(m_pEditorLayer = new EditorLayer());
	}

	bool Application::InitializeAppForWindows(HINSTANCE & hInstance, int nCmdShow)
	{
		m_pWindow = std::unique_ptr<Window>(Window::Create());
		m_pWindow->SetEventCallback(IE_BIND_EVENT_FN(Application::OnEvent));

		WindowsWindow* pWindow = (WindowsWindow*)m_pWindow.get();
		pWindow->SetWindowsSessionProps(hInstance, nCmdShow);
		if (!pWindow->Init(WindowProps())) {
			IE_CORE_FATAL(L"Fatal Error: Failed to initialize window.");
			return false;
		}

		Renderer::eTargetRenderAPI API = Renderer::eTargetRenderAPI::D3D_11;

//#define TEST_D3D12

#if defined TEST_D3D12
		API = Renderer::eTargetRenderAPI::D3D_12;
#endif
		Renderer::SetAPIAndCreateContext(API);
		Renderer::Init();

#if defined TEST_D3D12
		if (!Init()) { // Disabled for d3d11 impl
			IE_CORE_FATAL(L"Fatal Error: Failed to initiazlize application for Windows.");
			return false;
		}
#endif

		pWindow->PostInit();
		return true;
	}

	Application::~Application()
	{
		Shutdown();
	}

	bool Application::Init()
	{
		FileSystem::Init(ProjectName);

		m_pGameLayer = new GameLayer();

		std::string DocumentPath = FileSystem::ProjectDirectory;
		DocumentPath += "/Assets/Scenes/";
		DocumentPath += TargetSceneName;
		
		if (!m_pGameLayer->LoadScene(DocumentPath)) {
			throw ieException("Failed to initialize scene");
		}
		PushEngineLayers();

		IE_CORE_TRACE("Application Initialized");
		return true;
	}

	void Application::Run()
	{
		IE_ADD_FOR_GAME_DIST(
			BeginPlay(AppBeginPlayEvent{})
		);

		while(m_Running) {

			m_FrameTimer.Tick();
			const float& DeltaTime = (float)m_FrameTimer.DeltaTime();

#if defined TEST_D3D12
			m_pWindow->SetWindowTitleFPS(m_FrameTimer.FPS());
#endif

			m_pWindow->OnUpdate(DeltaTime);
#if defined TEST_D3D12
			m_pGameLayer->Update(DeltaTime); // Disabled for d3d11 impl
#endif

#ifndef TEST_D3D12
			//TEMP
			{
				Renderer::OnUpdate(DeltaTime);
				Renderer::OnPreFrameRender();
				Renderer::OnRender();
				Renderer::OnMidFrameRender();
			}
#endif

#if defined TEST_D3D12
			for (Layer* layer : m_LayerStack) { // Disabled for d3d11 impl
				layer->OnUpdate(DeltaTime);
			}

			m_pGameLayer->PreRender(); // Disabled for d3d11 impl
			m_pGameLayer->Render(); // Disabled for d3d11 impl

			// Render Editor UI
			IE_STRIP_FOR_GAME_DIST( // Disabled for d3d11 impl
				m_pImGuiLayer->Begin();
				for (Layer* layer : m_LayerStack) {
					layer->OnImGuiRender();
				}
				m_pGameLayer->OnImGuiRender();
				m_pImGuiLayer->End();
			);

			m_pGameLayer->PostRender(); // Disabled for d3d11 impl
#endif
			m_pWindow->EndFrame();
		}
	}

	void Application::Shutdown()
	{
	}

	void Application::OnEvent(Event & e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(IE_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(IE_BIND_EVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<WindowToggleFullScreenEvent>(IE_BIND_EVENT_FN(Application::OnWindowFullScreen));
		dispatcher.Dispatch<SceneSaveEvent>(IE_BIND_EVENT_FN(Application::SaveScene));
		dispatcher.Dispatch<AppBeginPlayEvent>(IE_BIND_EVENT_FN(Application::BeginPlay));
		dispatcher.Dispatch<AppEndPlayEvent>(IE_BIND_EVENT_FN(Application::EndPlay));
		dispatcher.Dispatch<AppScriptReloadEvent>(IE_BIND_EVENT_FN(Application::ReloadScripts));

		Input::GetInputManager().OnEvent(e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled())
				break;
		}
	}

	void Application::PushEngineLayers()
	{
		IE_STRIP_FOR_GAME_DIST(PushOverlay(m_pImGuiLayer);)
		IE_STRIP_FOR_GAME_DIST(PushOverlay(m_pEditorLayer);)
	}

	void Application::PushLayer(Layer * layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer * layer)
	{
		m_LayerStack.PushOverLay(layer);
		layer->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent & e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		m_pWindow->Resize(e.GetWidth(), e.GetHeight(), e.GetIsMinimized());
		return true;
	}

	bool Application::OnWindowFullScreen(WindowToggleFullScreenEvent& e)
	{
		m_pWindow->ToggleFullScreen(e.GetFullScreenEnabled());
		return true;
	}

	bool Application::SaveScene(SceneSaveEvent& e)
	{
		return FileSystem::WriteSceneToJson(m_pGameLayer->GetScene());
	}

	bool Application::BeginPlay(AppBeginPlayEvent& e)
	{
		m_pGameLayer->BeginPlay();
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
		IE_CORE_INFO("Reload Scirpts");
		ResourceManager::Get().GetMonoScriptManager().ReCompile();
		return true;
	}

}
