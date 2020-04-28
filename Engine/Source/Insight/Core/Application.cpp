#include "ie_pch.h"

#include "Application.h"

#include "Platform/Windows/Windows_Window.h"
#include "Insight/Input/Input.h"

#include "Insight/ImGui/ImGui_Layer.h"

//TEMP
#include "Insight/Runtime/AActor.h"

namespace Insight {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		IE_ASSERT(!s_Instance, "Trying to create Application instance when one already exists!");
		s_Instance = this;

		m_ImGuiLayer = new ImGuiLayer();
	}

	bool Application::InitializeAppForWindows(HINSTANCE & hInstance, int nCmdShow)
	{
		m_pWindow = std::unique_ptr<Window>(Window::Create());
		m_pWindow->SetEventCallback(IE_BIND_EVENT_FN(Application::OnEvent));

		static_cast<WindowsWindow*>(m_pWindow.get())->SetWindowsSessionProps(hInstance, nCmdShow);
		if (!static_cast<WindowsWindow*>(m_pWindow.get())->Init(WindowProps()))	
		{
			IE_CORE_FATAL(L"Fatal Error: Failed to initialize window.");
			return false;
		}

		if (!Init())
		{
			IE_CORE_FATAL(L"Fatal Error: Failed to initiazlize application for Windows.");
			return false;
		}

		return true;
	}

	Application::~Application()
	{
		Shutdown();
	}

	bool Application::Init()
	{
		PushEngineLayers();

		IE_CORE_TRACE("Application Initialized");
		return true;
	}

	void Application::Run()
	{
		AActor* actor = new AActor();
		while(m_Running)
		{
			m_FrameTimer.tick();
			const float& time = m_FrameTimer.seconds();
			const float& deltaTime = m_FrameTimer.dt();

			// m_Scene->OnUpdate(deltaTime);

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(deltaTime);

			m_pWindow->OnFramePreRender();
			m_pWindow->OnUpdate(deltaTime);
			m_pWindow->OnRender();

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();
			
			m_pWindow->ExecuteDraw();
		}
		delete actor;
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
		PushOverlay(m_ImGuiLayer);
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

}
