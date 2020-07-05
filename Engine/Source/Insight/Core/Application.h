#pragma once

#include <Insight/Core.h>

#include "Window.h"
#include "Insight/Systems/Time.h"
#include "Insight/Core/Scene/Scene.h"
#include "Insight/Layer_Types/ImGui_Layer.h"
#include "Insight/Layer_Types/Game_Layer.h"
#include "Insight/Layer_Types/Editor_Layer.h"
#include "Insight/Core/Layer/Layer_Stack.h"
#include "Insight/Events/Application_Event.h"

namespace Insight {


	class INSIGHT_API Application
	{
	public:
		Application();
		virtual ~Application();

		inline static Application& Get() { return *s_Instance; }

		bool InitializeAppForWindows(HINSTANCE& hInstance, int nCmdShow);
		bool Init();
		void Run();
		void Shutdown();

		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline GameLayer& GetGameLayer() { return *m_pGameLayer; }
		inline EditorLayer& GetEditorLayer() { return *m_pEditorLayer; }
		inline Window& GetWindow() { return *m_pWindow; }
		inline FrameTimer& GetFrameTimer() { return m_FrameTimer; }

		inline static bool IsPlaySessionUnderWay() { return s_Instance->m_pGameLayer->IsPlaySesionUnderWay(); }
		
	private:
		void PushEngineLayers();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowFullScreen(WindowToggleFullScreenEvent& e);
		//TEMP
		bool SaveScene(SceneSaveEvent& e);
		bool BeginPlay(AppBeginPlayEvent& e);
		bool EndPlay(AppEndPlayEvent& e);
	private:
		std::unique_ptr<Window>	m_pWindow;
		IE_STRIP_FOR_GAME_DIST(ImGuiLayer* m_pImGuiLayer = nullptr;)
		IE_STRIP_FOR_GAME_DIST(EditorLayer* m_pEditorLayer = nullptr;)
		GameLayer*				m_pGameLayer;
		bool					m_Running = true;
		LayerStack				m_LayerStack;
		FrameTimer				m_FrameTimer;
	private:
		static Application*		s_Instance;
	};

	// To be defined in client
	Application* CreateApplication();

}

