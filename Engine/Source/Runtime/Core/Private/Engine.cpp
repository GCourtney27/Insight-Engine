// Copyright Insight Interactive. All Rights Reserved.
#include <Engine_pch.h>

#include "Core/Public/Engine.h"

#include "GameFramework/AActor.h"
#include "Core/Public/Exception.h"
#include "Graphics/Renderer.h"
#include "EngineDefines.h"

#if IE_WIN32
#include "Platform/DirectX11/Wrappers/D3D11ImGuiLayer.h"
#include "Platform/DirectX12/Wrappers/D3D12ImGuiLayer.h"
#include "Platform/Win32/Win32Window.h"
#endif

#define IE_RENDER_MULTI_PLATFORM 1

#if IE_RENDER_MULTI_PLATFORM
#include "Graphics/Public/RenderCore.h"
#endif // IE_RENDER_MULTI_PLATFORM

#include "Core/Public/ieObject/ieStaticMeshActor.h"
#include "Core/Public/ieObject/Components/ieCameraComponent.h"
#include "Core/Public/ieObject/iePlayerCharacter.h"
#include "Core/Public/ieObject/ieCameraActor.h"

static const char* TargetSceneName = "Debug.iescene";
namespace Insight {


	Engine* Engine::s_Instance = nullptr;

	Engine::Engine()
		: m_WorldRenderer(&m_World, m_World.GetEntityAdmin())
	{
		IE_ASSERT(!s_Instance, "Trying to create Application instance when one already exists!");
		s_Instance = this;


#if IE_WITH_EDITOR
		// Initialize the core logger.
		if (!Debug::Logger::Initialize())
		{
			throw ieException(TEXT("Failed to initialize core logger"), ieException::EExceptionCategory::EC_Engine);
		}
#endif
	}

	Engine::~Engine()
	{
	}

	void Engine::DumpApp()
	{
		Debug::Logger::AppendMessageForCoreDump(TEXT("Core dump requested by engine."));
		Debug::Logger::InitiateCoreDump();
	}

	void Engine::Initialize()
	{
		ScopedMilliSecondTimer(TEXT("Core engine init"));

		// Initize the main file system.
		FileSystem::Init();
		
#if IE_RENDER_MULTI_PLATFORM
		m_World.Initialize(m_pWindow);

		ieCameraActor* pDebugCamera = m_World.CreateActor<ieCameraActor>();
		pDebugCamera->GetSubobject<ieCameraComponent>()->SetProjectionValues(80.f, (float)m_pWindow->GetWidth(), (float)m_pWindow->GetHeight(), 0.01f, 10000.f);
		m_World.SetSceneRenderCamera(pDebugCamera->GetCamera());
		
		m_WorldRenderer.Initialize(m_pWindow, Graphics::RB_Direct3D12);

		// Mesh database
		// Submesh name -> Owning file filename
		// Submesh name -> Owning file filename
		// ...

		// Mesh Manager
		/*
			[in] SubmeshName

			Filename = MeshDatabase[Submesh name]
			
			MeshFile = LoadMesh(Filename);
			Foreach submesh in MeshFile
				if submesh.name == SubmeshName
					ParseMeshfile and return MeshRef
		*/

		// Load Assets. TODO: This would come from a level file
		// Load Models
		Graphics::g_StaticGeometryManager.LoadFBXFromFile(L"Content/Models/Cube.fbx");
		Graphics::g_StaticGeometryManager.LoadFBXFromFile(L"Content/Models/Quad.fbx");
		// Load Materials
		Graphics::g_MaterialManager.LoadMaterialFromFile(L"M_DefaultMat.ieMat");
		Graphics::g_MaterialManager.LoadMaterialFromFile(L"M_RustedMetal.ieMat");
		// Load Textures
		// TODO: Only dds textures can be loaded right now.
		Graphics::ITextureRef AlbedoTexture = Graphics::g_pTextureManager->LoadTexture(L"Content/Textures/RustedIron/RustedIron_Albedo.dds", Graphics::DT_Magenta2D, false);
		Graphics::ITextureRef NormalTexture = Graphics::g_pTextureManager->LoadTexture(L"Content/Textures/RustedIron/RustedIron_Normal.dds", Graphics::DT_Magenta2D, false);
		//Graphics::ITextureRef DebugTexture = Graphics::g_pTextureManager->LoadTexture(L"Content/Textures/Debug/DebugCheckerBoard_Albedo.dds", Graphics::DT_Magenta2D, false);
		
		// Create references to loaded assets
		StaticMeshGeometryRef CubeMesh = Graphics::g_StaticGeometryManager.GetStaticMeshByName(L"Cube");
		StaticMeshGeometryRef QuadMesh = Graphics::g_StaticGeometryManager.GetStaticMeshByName(L"Plane");
		MaterialRef RustedMetalMat = Graphics::g_MaterialManager.GetMaterialByName(L"M_RustedMetal");
		RustedMetalMat->SetAlbedoTexture(AlbedoTexture);
		RustedMetalMat->SetNormalTexture(NormalTexture);
		MaterialRef DefaultMat = Graphics::g_MaterialManager.GetMaterialByName(L"M_DefaultMat");
		DefaultMat->SetColor(FVector4(.6f, .6f, .6f, 1.f));

		ieStaticMeshActor* pCubeActor = m_World.CreateActor<ieStaticMeshActor>();
		pCubeActor->GetTransform().SetPosition(0.f, 50.f, 50.f);
		pCubeActor->GetTransform().SetScale(20.f, 20.f, 20.f);
		ieStaticMeshComponent* pSMCubeActor = pCubeActor->GetSubobject<ieStaticMeshComponent>();
		pSMCubeActor->SetMaterial(RustedMetalMat);
		pSMCubeActor->SetMesh(CubeMesh);

		ieStaticMeshActor* pQuadActor = m_World.CreateActor<ieStaticMeshActor>();
		pQuadActor->GetTransform().SetPosition(0.f, -50.f, 50.f);
		pQuadActor->GetTransform().SetScale(800.f, 800.f, 800.f);
		ieStaticMeshComponent* pSMQuadActor = pQuadActor->GetSubobject<ieStaticMeshComponent>();
		pSMQuadActor->SetMaterial(DefaultMat);
		pSMQuadActor->SetMesh(QuadMesh);

		

		iePlayerCharacter* pPlayerCharacter = m_World.CreateActor<iePlayerCharacter>();
		

		m_World.BeginPlay();

		float DeltaMs = 0.f;
		while (m_Running)
		{
			m_AppTimer.Tick();
			DeltaMs = m_AppTimer.DeltaTime();

			m_pWindow->OnUpdate();

			// Update the input system. 
			m_InputDispatcher.UpdateInputs(DeltaMs);

			m_World.TickWorld(DeltaMs);
			m_World.ExecuteCoreSystems();

			// TODO if(SimulationActive)
				m_World.ExecuteGameplaySystems();

			m_WorldRenderer.Render();
			// m_UI.Render();

			//IE_LOG(Log, TEXT("FPS: %f"), m_AppTimer.FPS());
		}
		Graphics::g_pCommandManager->IdleGPU();

		exit(EXIT_SUCCESS); // Just for easier debugging quit the entire program.
#endif

		// Create and initialize the renderer.
		Renderer::SetSettingsAndCreateContext(FileSystem::LoadGraphicsSettingsFromJson(), m_pWindow);

		// Create the game layer that will host all game logic.
		m_pGameLayer = new GameLayer();

		// Load the Scene
		std::string DocumentPath = StringHelper::WideToString(FileSystem::GetRelativeContentDirectoryW(L"/Scenes/"));
		DocumentPath += TargetSceneName;
		if (!m_pGameLayer->LoadScene(DocumentPath)) {
			throw ieException(TEXT("Failed to initialize scene"));
		}
		Renderer::SetActiveCamera(&m_pGameLayer->GetScene()->GetSceneCamera());

		// Push core engine layers to the layer stack
		PushCoreLayers();
	}

	void Engine::PostInit()
	{
		Renderer::PostInit();

		m_pWindow->PostInit();
		ResourceManager::Get().PostAppInit();
		m_pGameLayer->PostInit();

		IE_LOG(Verbose, TEXT("App Initialized"));
	}

	static bool s_ReloadRuntime = false;
	float g_GPUThreadFPS = 0.0f;
	void Engine::RenderThread()
	{
		while (m_Running)
		{
			if (m_IsSuspended) continue;

			m_GraphicsThreadTimer.Tick();
			g_GPUThreadFPS = m_GraphicsThreadTimer.FPS();

			Renderer::OnUpdate(m_GraphicsThreadTimer.DeltaTime());

			// Prepare for rendering. 
			Renderer::OnPreFrameRender();

			// Render the world. 
			Renderer::OnRender();

			// Render the Editor/UI last. 
#if IE_WITH_EDITOR
			m_pImGuiLayer->Begin();
			for (Layer* pLayer : m_LayerStack)
				pLayer->OnImGuiRender();
			m_pGameLayer->OnImGuiRender();
			Renderer::OnEditorRender();
			m_pImGuiLayer->End();
#endif

			// Submit for draw and present. 
			Renderer::ExecuteDraw();
			Renderer::SwapBuffers();
		}
	}

	EErrorCode Engine::Run()
	{
		BeginPlay(AppBeginPlayEvent{});

		// Put all rendering on another thread. 
		std::thread RenderThread(&Engine::RenderThread, this);

		while (m_Running)
		{
			if (m_IsSuspended) continue;

			if (m_pWindow->GetIsVisible())
			{
				m_GameThreadTimer.Tick();
				float DeltaMs = m_GameThreadTimer.DeltaTime();

				// Process the window's Messages 
				m_pWindow->OnUpdate();

				// Update the input system. 
				m_InputDispatcher.UpdateInputs(DeltaMs);

				// Update game logic. 
				m_pGameLayer->Update(DeltaMs);

				// Update the layer stack. 
				for (Layer* pLayer : m_LayerStack)
					pLayer->OnUpdate(DeltaMs);
			}
			else
			{
				m_pWindow->BackgroundUpdate();
			}
		}

		// Close the render thread and flush the GPU.
		RenderThread.join();

		// Shutdown the engine and release all resources.
		Shutdown();

		return EC_Success;
	}

	EErrorCode Engine::RunSingleThreaded()
	{
		{
			m_GameThreadTimer.Tick();
			float DeltaMs = m_GameThreadTimer.DeltaTime();

			// Process the window's Messages 
			m_pWindow->OnUpdate();

			{
				static FrameTimer GraphicsTimer;
				GraphicsTimer.Tick();
				g_GPUThreadFPS = GraphicsTimer.FPS();

				Renderer::OnUpdate(GraphicsTimer.DeltaTime());

				// Prepare for rendering. 
				Renderer::OnPreFrameRender();

				// Render the world. 
				Renderer::OnRender();

				// Render the Editor/UI last. 
#if IE_WITH_EDITOR
				m_pImGuiLayer->Begin();
				for (Layer* pLayer : m_LayerStack)
					pLayer->OnImGuiRender();
				m_pGameLayer->OnImGuiRender();
				m_pImGuiLayer->End();
#endif

				// Submit for draw and present. 
				Renderer::ExecuteDraw();
				Renderer::SwapBuffers();
			}

			m_pWindow->OnUpdate();

			// Update the input system. 
			m_InputDispatcher.UpdateInputs(DeltaMs);

			// Update game logic. 
			m_pGameLayer->Update(DeltaMs);

			// Update the layer stack. 
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(DeltaMs);
		}
		return EC_Success;
	}

	void Engine::Shutdown()
	{
		Renderer::Destroy();
		m_pWindow->Shutdown();
	}

	void Engine::PushCoreLayers()
	{
#if (IE_WITH_EDITOR)
		switch (Renderer::GetAPI())
		{
		case Renderer::ETargetRenderAPI::Direct3D_11:
			m_pImGuiLayer = new D3D11ImGuiLayer();
			PushOverlay(m_pImGuiLayer);
			break;
		case Renderer::ETargetRenderAPI::Direct3D_12:
			m_pImGuiLayer = new D3D12ImGuiLayer();
			PushOverlay(m_pImGuiLayer);
			break;
		default:
			IE_LOG(Error, TEXT("Failed to create ImGui layer in engine with API of type \"%i\" Or engine has disabled editor."), Renderer::GetAPI());
			break;
		}

		m_pEditorLayer = new EditorLayer();
		PushOverlay(m_pEditorLayer);
#endif

		m_pPerfOverlay = new PerfOverlay();
		PushOverlay(m_pPerfOverlay);
	}

	void Engine::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Engine::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverLay(layer);
		layer->OnAttach();
	}



	// -----------------
	// Events Callbacks |
	// -----------------

	void Engine::OnEvent(Event& e)
	{
		EventDispatcher Dispatcher(e);
		Dispatcher.Dispatch<WindowCloseEvent>(IE_BIND_LOCAL_EVENT_FN(Engine::OnWindowClose));
		Dispatcher.Dispatch<WindowResizeEvent>(IE_BIND_LOCAL_EVENT_FN(Engine::OnWindowResize));
		Dispatcher.Dispatch<WindowToggleFullScreenEvent>(IE_BIND_LOCAL_EVENT_FN(Engine::OnWindowFullScreen));
		Dispatcher.Dispatch<SceneSaveEvent>(IE_BIND_LOCAL_EVENT_FN(Engine::SaveScene));
		Dispatcher.Dispatch<AppBeginPlayEvent>(IE_BIND_LOCAL_EVENT_FN(Engine::BeginPlay));
		Dispatcher.Dispatch<AppEndPlayEvent>(IE_BIND_LOCAL_EVENT_FN(Engine::EndPlay));
		Dispatcher.Dispatch<AppScriptReloadEvent>(IE_BIND_LOCAL_EVENT_FN(Engine::ReloadScripts));
		Dispatcher.Dispatch<ShaderReloadEvent>(IE_BIND_LOCAL_EVENT_FN(Engine::ReloadShaders));

		// Process input event callbacks. 
		m_InputDispatcher.ProcessInputEvent(e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
		}
	}

	bool Engine::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Engine::OnWindowResize(WindowResizeEvent& e)
	{
		m_pWindow->Resize(e.GetWidth(), e.GetHeight(), e.GetIsMinimized());
#if !IE_RENDER_MULTI_PLATFORM
		Renderer::PushEvent<WindowResizeEvent>(e);
#endif

		return true;
	}

	bool Engine::OnWindowFullScreen(WindowToggleFullScreenEvent& e)
	{
		if (m_pWindow->GetWindowMode() == EWindowMode::WM_FullScreen)
			m_pWindow->SetWindowMode(EWindowMode::WM_Windowed);
		else
			m_pWindow->SetWindowMode(EWindowMode::WM_FullScreen);

#if !IE_RENDER_MULTI_PLATFORM
		Renderer::PushEvent<WindowToggleFullScreenEvent>(e);
#endif
		return true;
	}

	bool Engine::OnAppSuspendingEvent(AppSuspendingEvent& e)
	{
		m_IsSuspended = true;
		return true;
	}

	bool Engine::OnAppResumingEvent(AppResumingEvent& e)
	{
		m_IsSuspended = false;
		return true;
	}

	bool Engine::SaveScene(SceneSaveEvent& e)
	{
		std::future<bool> Future = std::async(std::launch::async, FileSystem::WriteSceneToJson, m_pGameLayer->GetScene());
		return true;
	}

	bool Engine::BeginPlay(AppBeginPlayEvent& e)
	{
		PushLayer(m_pGameLayer);
		return true;
	}

	bool Engine::EndPlay(AppEndPlayEvent& e)
	{
		m_pGameLayer->EndPlay();
		m_LayerStack.PopLayer(m_pGameLayer);
		m_pGameLayer->OnDetach();
		return true;
	}

	bool Engine::ReloadScripts(AppScriptReloadEvent& e)
	{
		IE_LOG(Log, TEXT("Reloading C# Scripts"));
#if IE_WIN32
		ResourceManager::Get().GetMonoScriptManager().ReCompile();
#endif
		return true;
	}

	bool Engine::ReloadShaders(ShaderReloadEvent& e)
	{
		//Renderer::OnShaderReload(); 
		Renderer::PushEvent<ShaderReloadEvent>(e);
		return true;
	}
}
