// Copyright Insight Interactive. All Rights Reserved.
/*
	File - Application.h
	Source - Application.cpp

	Author - Garrett Courtney

	Purpose:
	Core application the engine uses to execute.

	Description:
	The application holds the layers that make up the application. Layers give functionality to the application
	whether it be game code, editor code etc. These are the core components of any program using the Insight api.

*/

#pragma once

#include <Insight/Core.h>

#include "Insight/Core/Window.h"
#include "Insight/Core/Scene/Scene.h"
#include "Insight/Systems/Frame_Timer.h"
#include "Insight/Core/Layer/Layer_Stack.h"

#include "Insight/Events/Application_Event.h"
#include "Insight/Input/Input_Dispatcher.h"

#include "Insight/Core/Layer/Game_Layer.h"
#include "Insight/Core/Layer/ImGui_Layer.h"
#include "Insight/Core/Layer/Editor_Layer.h"
#include "Insight/Core/Layer/Perf_Monitor_Layer.h"


namespace Insight {


	class INSIGHT_API Application
	{
	public:
		Application();
		virtual ~Application();

		Application(Application& App) = delete;
		Application(Application&& App) = delete;

		inline static Application& Get() { return *s_Instance; }

		// Initialize the core components of the application. Should be called once
		// at the beginning of the application, after the window has been initialized.
		virtual bool InitializeCoreApplication();
		// Called when the main portion of the applicaiton has been initialized.
		virtual void PostInit();
		// Main loop of the application. This is the main entry point for every frame.
		virtual void Run(float DeltaMs);
		// Shutdown the application and release all resources.
		virtual void Shutdown();

		void OnEvent(Event& e);
		// Add a layer to the application's layer stack.
		void PushLayer(Layer* layer);
		// Push an overlay to the front of the application's layer stack.
		void PushOverlay(Layer* layer);

		inline Scene& GetScene() const { return *(m_pGameLayer->GetScene()); }
		// Get the applications core layer stack.
		inline LayerStack& GetLayerStack() { return m_LayerStack; }
		// Get the ImGui UI layer.
		inline ImGuiLayer& GetImGuiLayer() { return *m_pImGuiLayer; }
		// Get the game layer that handles the update logic for the runtime components.
		// Should only ever be used for editor actions.
		inline GameLayer& GetGameLayer() { return *m_pGameLayer; }
		// Get the editor layer for the application.
		IE_STRIP_FOR_GAME_DIST(inline EditorLayer& GetEditorLayer() { return *m_pEditorLayer; })

		// Returns true if the editor is currently simmulating a game session.
		inline static bool IsPlaySessionUnderWay() { return s_Instance->m_pGameLayer->IsPlaySesionUnderWay(); }
		
	private:
		void PushCoreLayers();

		bool SaveScene(SceneSaveEvent& e);
		bool BeginPlay(AppBeginPlayEvent& e);
		bool EndPlay(AppEndPlayEvent& e);
		bool ReloadScripts(AppScriptReloadEvent& e);
	protected:
		IE_STRIP_FOR_GAME_DIST(ImGuiLayer* m_pImGuiLayer; )
		IE_STRIP_FOR_GAME_DIST(EditorLayer* m_pEditorLayer; )
		PerfOverlay*			m_pPerfOverlay;
		GameLayer*				m_pGameLayer;
		LayerStack				m_LayerStack;
	private:
		static Application*		s_Instance;
	};

	// To be defined by client.
	Application* CreateApplication();

}

