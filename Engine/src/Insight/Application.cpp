#include "ie_pch.h"

#include "Application.h"
#include "Platform/Windows/Windows_Window.h"
#include "Insight/Log.h"
#include "Insight/Input/Input.h"

#include "Insight/Editor/ImGui_Layer.h"

namespace Insight {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		IE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		Input::IsKeyPressed('A'); // Removing this will gause mouse buffer to throw error. Why?
	}

	void Application::InitializeAppForWindows(HINSTANCE & hInstance, int nCmdShow)
	{
		m_pWindow = std::unique_ptr<Window>(Window::Create());
		m_pWindow->SetEventCallback(IE_BIND_EVENT_FN(Application::OnEvent));

		static_cast<WindowsWindow*>(m_pWindow.get())->SetWindowsSessionProps(hInstance, nCmdShow);
		if (!static_cast<WindowsWindow*>(m_pWindow.get())->Init(WindowProps()))	{
			IE_CORE_FATAL(L"Fatal Error: Failed to initialize window. Exiting.");
		}

		if (!Init())
		{
			IE_CORE_FATAL(L"Failed to initiazlize application");
		}

	}

	Application::~Application()
	{
	}

	bool Application::Init()
	{
		PushEngineLayers();
		return true;
	}

	void Application::Run()
	{
		while(m_Running)
		{
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			
			m_pWindow->OnUpdate();
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

		//IE_CORE_TRACE("{0}", e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled())
				break;
		}
	}

	void Application::PushEngineLayers()
	{
		//PushOverlay(new ImGuiLayer());
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
		m_pWindow->Resize(e.GetWidth(), e.GetHeight());
		return true;
	}

}
