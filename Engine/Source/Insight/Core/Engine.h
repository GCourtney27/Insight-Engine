#pragma once

#include <Insight/Core.h>

#include "Insight/Core/Log.h"
#include "Renderer/Renderer.h"
#include "Insight/Core/Application.h"
#include "Insight/Core/ie_Exception.h"
#include "Insight/Utilities/Profiling.h"
#include "Insight/Input/Input_Dispatcher.h"
#include "Insight/Events/Application_Event.h"


namespace Insight {

	class INSIGHT_API Engine
	{
	public:
		Engine();
		~Engine();
		
		int RunWin32App(Application* pApp, HINSTANCE hInstance, LPWSTR CmdArgs, int NumCmdArgs);
		
	protected:
		int RenderThread();
		int GameThread();

		void OnEvent(Event& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowFullScreen(WindowToggleFullScreenEvent& e);
		bool ReloadShaders(ShaderReloadEvent& e);

	protected:
		Application*			m_pApplication;
		bool					m_Running;
		std::unique_ptr<Window>	m_pWindow;
		FileSystem				m_FileSystem;
		Input::InputDispatcher	m_InputDispatcher;
		FrameTimer				m_GameThreadTimer;
		FrameTimer				m_GPUThreadTimer;
		//Renderer				m_Renderer;
	};
}
