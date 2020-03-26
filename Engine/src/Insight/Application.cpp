#include "ie_pch.h"

#include "Application.h"
#include "Platform/Windows/Windows_Window.h"

namespace Insight {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		
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
