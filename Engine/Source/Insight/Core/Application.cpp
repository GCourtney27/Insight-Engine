#include <Engine_pch.h>

#include "Application.h"

#include "Insight/Input/Input.h"
#include "Insight/Actors/AActor.h"
#include "Insight/Layer_Types/ImGui_Layer.h"
#include "Insight/Core/ie_Exception.h"
#include "Renderer/Renderer.h"

#if defined IE_PLATFORM_WINDOWS
#include "Renderer/Platform/Windows/DirectX_11/Wrappers/D3D11_ImGui_Layer.h"
#include "Renderer/Platform/Windows/DirectX_12/Wrappers/D3D12_ImGui_Layer.h"
#include "Platform/Windows/Windows_Window.h"
#endif

// TODO: Make the project hot swapable
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

	bool Application::InitializeAppForWindows(HINSTANCE& hInstance, int nCmdShow)
	{
		m_pWindow = std::unique_ptr<Window>(Window::Create());
		m_pWindow->SetEventCallback(IE_BIND_EVENT_FN(Application::OnEvent));

		WindowsWindow* pWindow = (WindowsWindow*)m_pWindow.get();
		pWindow->SetWindowsSessionProps(hInstance, nCmdShow);
		if (!pWindow->Init(WindowProps())) {
			IE_CORE_FATAL(L"Fatal Error: Failed to initialize window.");
			return false;
		}

		if (!InitializeCoreApplication()) {
			IE_CORE_FATAL(L"Fatal Error: Failed to initiazlize application for Windows.");
			return false;
		}

		return true;
	}

	Application::~Application()
	{
		Shutdown();
	}

	bool Application::InitializeCoreApplication()
	{
		// Initize the main file system
		FileSystem::Init(ProjectName);

		// Create and initialize the renderer
		Renderer::SetSettingsAndCreateContext(FileSystem::LoadGraphicsSettingsFromJson());
		Renderer::Init();

		// Create the main game layer
		m_pGameLayer = new GameLayer();

		// Load the Scene
		std::string DocumentPath(FileSystem::GetProjectDirectory());
		DocumentPath += "/Assets/Scenes/";
		DocumentPath += TargetSceneName;
		if (!m_pGameLayer->LoadScene(DocumentPath)) {
			throw ieException("Failed to initialize scene");
		}

		// Push core app layers to the layer stack
		PushEngineLayers();

		return true;
	}

	void Application::PostInit()
	{
		Renderer::PostInit();

		m_pWindow->PostInit();

		ResourceManager::Get().PostAppInit();
		IE_CORE_TRACE("Application Initialized");

		m_pGameLayer->PostInit();

		m_AppInitialized = true;
	}

	void Application::RenderThread()
	{
		FrameTimer GraphicsTimer;
		float DeltaMs = 0.0f;

		while (m_Running)
		{
			GraphicsTimer.Tick();
			DeltaMs = GraphicsTimer.DeltaTime();

			Renderer::OnUpdate(DeltaMs);

			// Prepare for the render pass.
			Renderer::OnPreFrameRender();

			// Render the world.
			Renderer::OnRender();
			Renderer::OnMidFrameRender();

			// Render the Editor/UI last.
			IE_STRIP_FOR_GAME_DIST
			(
				m_pImGuiLayer->Begin();
				for (Layer* pLayer : m_LayerStack)
					pLayer->OnImGuiRender();
				m_pGameLayer->OnImGuiRender();
				m_pImGuiLayer->End();
			);

			// Submit for draw.
			Renderer::ExecuteDraw();
			Renderer::SwapBuffers();
		}
	}

	void Application::Run()
	{
		IE_ADD_FOR_GAME_DIST(
			BeginPlay(AppBeginPlayEvent{})
		);

		std::thread RenderThread(&Application::RenderThread, this);
		
		while (m_Running) 
		{
			m_FrameTimer.Tick();
			float DeltaMs = m_FrameTimer.DeltaTime();
			m_pWindow->SetWindowTitleFPS(m_FrameTimer.FPS());

			m_pWindow->OnUpdate(DeltaMs);
			m_pGameLayer->Update(DeltaMs);

			for (Layer* layer : m_LayerStack) 
				layer->OnUpdate(DeltaMs);
		}

		RenderThread.join();
	}

	void Application::Shutdown()
	{
		Renderer::Destroy();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher Dispatcher(e);
		Dispatcher.Dispatch<WindowCloseEvent>(IE_BIND_EVENT_FN(Application::OnWindowClose));
		Dispatcher.Dispatch<WindowResizeEvent>(IE_BIND_EVENT_FN(Application::OnWindowResize));
		Dispatcher.Dispatch<WindowToggleFullScreenEvent>(IE_BIND_EVENT_FN(Application::OnWindowFullScreen));
		Dispatcher.Dispatch<SceneSaveEvent>(IE_BIND_EVENT_FN(Application::SaveScene));
		Dispatcher.Dispatch<AppBeginPlayEvent>(IE_BIND_EVENT_FN(Application::BeginPlay));
		Dispatcher.Dispatch<AppEndPlayEvent>(IE_BIND_EVENT_FN(Application::EndPlay));
		Dispatcher.Dispatch<AppScriptReloadEvent>(IE_BIND_EVENT_FN(Application::ReloadScripts));
		Dispatcher.Dispatch<ShaderReloadEvent>(IE_BIND_EVENT_FN(Application::ReloadShaders));

		Input::GetInputManager().OnEvent(e);
		Runtime::ACamera::Get().OnEvent(e);

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
#if defined IE_PLATFORM_WINDOWS
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

		IE_STRIP_FOR_GAME_DIST(m_pEditorLayer = new EditorLayer());
		IE_STRIP_FOR_GAME_DIST(PushOverlay(m_pImGuiLayer);)
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

	bool Application::OnWindowClose(WindowCloseEvent& e)
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

	bool Application::ReloadShaders(ShaderReloadEvent& e)
	{
		Renderer::OnShaderReload();
		return true;
	}

}
