#pragma once

#include "Insight/Core.h"

#include "Window.h"
#include "Insight/Core/Layer/Layer_Stack.h"
#include "Insight/Events/Application_Event.h"
#include "Insight/Editor/ImGui_Layer.h"

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
		bool InitializeAppForWindows(HINSTANCE& hInstance, int nCmdShow);
		inline static Application& Get() { return *s_Instance; }
	private:
		void PushEngineLayers();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowFullScreen(WindowToggleFullScreenEvent& e);
	private:
		std::unique_ptr<Window> m_pWindow;
		ImGuiLayer* m_ImGuiLayer = nullptr;
		bool m_Running = true;
		LayerStack m_LayerStack;
	private:
		static Application* s_Instance;
	};

	// To be defined in client
	Application* CreateApplication();

}

