#pragma once

#include "Core.h"

#include "Window.h"
#include "Layer_Stack.h"
#include "Events/Application_Event.h"

namespace Insight {


	class INSIGHT_API Application
	{
	public:
		Application();
		virtual ~Application();

		bool Init();

		void Run();
		void Shutdown();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_pWindow; }
		void InitializeAppForWindows(HINSTANCE& hInstance, int nCmdShow);
		inline static Application& Get() { return *s_Instance; }
	private:
		void PushEngineLayers();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_pWindow;
		bool m_Running = true;
		LayerStack m_LayerStack;
	private:
		static Application* s_Instance;
	};

	// To be defined in client
	Application* CreateApplication();

}

