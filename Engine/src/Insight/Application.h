#pragma once

#include "Core.h"

#include "Window.h"
#include "Events/ApplicationEvent.h"

namespace Insight {


	class INSIGHT_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void CreateConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns);

		inline Window& GetWindow() { return *m_pWindow; }
		void InitializeWindow(HINSTANCE& hInstance, int nCmdShow);
		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		std::unique_ptr<Window> m_pWindow;
		bool m_Running = true;

	private:
		static Application* s_Instance;
	};

	// To be defined in client
	Application* CreateApplication();

}

