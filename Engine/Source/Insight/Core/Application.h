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

		// Initialie a new application for the windows platform.
		bool InitializeAppForWindows(HINSTANCE& hInstance, int nCmdShow);
		// Initialize the core components of the application. Should be called once
		// at the beginning of the application, after the window has been initialized.
		bool Init();
		// Main loop of the application. This is the main entry point for every frame.
		void Run();
		// Shutdown the application and release all resources.
		void Shutdown();

		void OnEvent(Event& e);
		// Add a layer to the applications layer stack.
		void PushLayer(Layer* layer);
		// Remove a layer to the applications layer stack.
		void PushOverlay(Layer* layer);

		// Get the game layer that handles the update logic for the runtime components.
		// Should only every be used for editor actions.
		inline GameLayer& GetGameLayer() { return *m_pGameLayer; }
		// Get the editor layer for the application.
		IE_STRIP_FOR_GAME_DIST(inline EditorLayer& GetEditorLayer() { return *m_pEditorLayer; })
		// Get the main rendering window assocciated with the application.
		inline Window& GetWindow() { return *m_pWindow; }
		// Get the frame timer for the application.
		inline FrameTimer& GetFrameTimer() { return m_FrameTimer; }

		// Returns true if the editor is currently simmulating a game session.
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
		IE_STRIP_FOR_GAME_DIST( ImGuiLayer* m_pImGuiLayer = nullptr; )
		IE_STRIP_FOR_GAME_DIST( EditorLayer* m_pEditorLayer = nullptr; )
		GameLayer*				m_pGameLayer;
		bool					m_Running = true;
		LayerStack				m_LayerStack;
		FrameTimer				m_FrameTimer;
		FileSystem				m_FileSystem;
	private:
		static Application*		s_Instance;
	};

	// To be defined in client
	Application* CreateApplication();

}

