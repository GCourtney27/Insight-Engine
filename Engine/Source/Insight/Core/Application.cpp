#include <ie_pch.h>

#include "Application.h"

#include "Insight/Input/Input.h"
#include "Insight/Runtime/AActor.h"
#include "Insight/Layer_Types/ImGui_Layer.h"
#include "Platform/Windows/Windows_Window.h"
#include "Insight/Core/ieException.h"
#include "Insight/Rendering/Rendering_Context.h"

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

		((WindowsWindow*)m_pWindow.get())->SetWindowsSessionProps(hInstance, nCmdShow);
		if (!((WindowsWindow*)m_pWindow.get())->Init(WindowProps())) {
			IE_CORE_FATAL(L"Fatal Error: Failed to initialize window.");
			return false;
		}

		RenderingContext::SetAPIAndCreateContext(RenderingContext::eRenderingAPI::D3D_11);
		RenderingContext::Init();

		if (!Init()) {
			IE_CORE_FATAL(L"Fatal Error: Failed to initiazlize application for Windows.");
			return false;
		}

		((WindowsWindow*)m_pWindow.get())->PostInit();

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

		/*std::string DocumentPath = FileSystem::ProjectDirectory;
		DocumentPath += "/Assets/Scenes/";
		DocumentPath += TargetSceneName;
		
		if (!m_pGameLayer->LoadScene(DocumentPath)) {
			throw ieException("Failed to initialize scene");
		}
		PushEngineLayers();*/

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
			//m_pWindow->SetWindowTitleFPS(m_FrameTimer.FPS());

			m_pWindow->OnUpdate(DeltaTime);
			//m_pGameLayer->Update(DeltaTime);

			//TEMP
			{
				RenderingContext::OnUpdate(DeltaTime);
				RenderingContext::OnPreFrameRender();
				RenderingContext::OnMidFrameRender();
			}

			/*for (Layer* layer : m_LayerStack) {
				layer->OnUpdate(DeltaTime);
			}*/

			//m_pGameLayer->PreRender();
			//m_pGameLayer->Render();

			// Render Editor UI
			/*IE_STRIP_FOR_GAME_DIST(
				m_pImGuiLayer->Begin();
				for (Layer* layer : m_LayerStack) {
					layer->OnImGuiRender();
				}
				m_pGameLayer->OnImGuiRender();
				m_pImGuiLayer->End();
			);*/

			//m_pGameLayer->PostRender();
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
