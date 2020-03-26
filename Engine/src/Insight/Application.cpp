#include "ie_pch.h"

#include "Application.h"
#include "Platform/Windows/Windows_Window.h"
#include "Insight/Log.h"

namespace Insight {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		IE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
	}

	void Application::InitializeWindow(HINSTANCE & hInstance, int nCmdShow)
	{
		m_pWindow = std::unique_ptr<Window>(Window::Create());
		static_cast<WindowsWindow*>(m_pWindow.get())->SetWindowsApplicationInstance(hInstance);
		static_cast<WindowsWindow*>(m_pWindow.get())->SetCmdArgs(nCmdShow);
		static_cast<WindowsWindow*>(m_pWindow.get())->Init(WindowProps());
	}


	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_pWindow->ProccessWindowMessages())
		{

		}
	}

	bool Application::OnWindowClose(WindowCloseEvent & e)
	{
		m_Running = false;
		return true;
	}


}
