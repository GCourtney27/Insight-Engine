#include <ie_pch.h>

#include "Application.h"

#include "Insight/Input/Input.h"
#include "Insight/Runtime/AActor.h"
#include "Insight/Layer_Types/ImGui_Layer.h"
#include "Platform/Windows/Windows_Window.h"
#include "Insight/Core/ieException.h"
#include "Insight/Rendering/Renderer.h"

#if defined IE_PLATFORM_WINDOWS
#include "Platform/Windows/DirectX_12/D3D12_ImGui_Layer.h"
#include "Platform/Windows/DirectX_11/D3D11_ImGui_Layer.h"
#endif

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

		if (!InitCoreApplication()) {
			IE_CORE_FATAL(L"Fatal Error: Failed to initiazlize application for Windows.");
			return false;
		}

		pWindow->PostInit();
		return true;
	}

	Application::~Application()
	{
		Shutdown();
	}

	bool Application::InitCoreApplication()
	{
		// Initize the main file system
		FileSystem::Init(ProjectName);

		// Create and initialize the renderer
		Renderer::SetSettingsAndCreateContext(FileSystem::LoadGraphicsSettingsFromJson());
		Renderer::Init();

		// Create the main game layer
		m_pGameLayer = new GameLayer();

		// Load the Scene
		std::string DocumentPath = FileSystem::ProjectDirectory;
		DocumentPath += "/Assets/Scenes/";
		DocumentPath += TargetSceneName;
		if (!m_pGameLayer->LoadScene(DocumentPath)) {
			throw ieException("Failed to initialize scene");
		}
		
		// Push core app layer to the layer stack
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
			m_pWindow->SetWindowTitleFPS(m_FrameTimer.FPS());


			m_pWindow->OnUpdate(DeltaTime);
			m_pGameLayer->Update(DeltaTime);

			for (Layer* layer : m_LayerStack) { 
				layer->OnUpdate(DeltaTime);
			}

			m_pGameLayer->PreRender();
			m_pGameLayer->Render();

			// Render Editor UI
			IE_STRIP_FOR_GAME_DIST(
				m_pImGuiLayer->Begin();
				for (Layer* layer : m_LayerStack) {
					layer->OnImGuiRender();
				}
				m_pGameLayer->OnImGuiRender();
				m_pImGuiLayer->End();
			);

			m_pGameLayer->PostRender();
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
			if (e.Handled()) break;
		}
	}

	void Application::PushEngineLayers()
	{
		switch (Renderer::GetAPI())
		{
		case Renderer::eTargetRenderAPI::D3D_11:
		{
			IE_STRIP_FOR_GAME_DIST(m_pImGuiLayer = new D3D11ImGuiLayer());
			break;
		}
		case Renderer::eTargetRenderAPI::D3D_12:
		{
			IE_STRIP_FOR_GAME_DIST(m_pImGuiLayer = new D3D12ImGuiLayer());
			break;
		}
		default:
		{
			IE_CORE_ERROR("Failed to creat ImGui layer in application with API of type \"{0}\"", Renderer::GetAPI());
			break;
		}
		}

		IE_STRIP_FOR_GAME_DIST(m_pEditorLayer = new EditorLayer());
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
