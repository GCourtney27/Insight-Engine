// Copyright Insight Interactive. All Rights Reserved.
#include <Engine_pch.h>

#include "Application.h"

#include "Runtime/GameFramework/AActor.h"
#include "Runtime/Core/Exception.h"
#include "Runtime/Graphics/Renderer.h"

#if IE_PLATFORM_BUILD_WIN32
#include "Platform/DirectX11/Wrappers/D3D11ImGuiLayer.h"
#include "Platform/DirectX12/Wrappers/D3D12ImGuiLayer.h"
#include "Platform/Win32/Win32Window.h"
#endif

#if IE_PLATFORM_BUILD_WIN32
#define EDITOR_UI_ENABLED 1
#else
#define EDITOR_UI_ENABLED 0
#endif


#define IE_RENDER_MULTI_PLATFORM 1

#if IE_RENDER_MULTI_PLATFORM
#include "Runtime/Graphics/Public/RenderCore.h"
// TEMP
#include "Runtime/Graphics/Private/ISwapChain.h"
#include "Runtime/Graphics/Public/IDevice.h"
#endif // IE_RENDER_MULTI_PLATFORM

#include "Platform/DirectX12/Public/Resource/D3D12ColorBuffer.h"
#include "Platform/DirectX12/Public/Resource/D3D12VertexBuffer.h"
#include "Platform/DirectX12/Public/Resource/D3D12IndexBuffer.h"
#include "Platform/DirectX12/Public/D3D12PipelineState.h"
#include "Platform/DirectX12/Public/D3D12RootSignature.h"

#include "Platform/DirectX12/Wrappers/D3D12Shader.h"
#include <d3dcompiler.h>
static const char* TargetSceneName = "Debug.iescene";
namespace Insight {


	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		IE_ASSERT(!s_Instance, "Trying to create Application instance when one already exists!");
		s_Instance = this;

		// Initialize the core logger.
		IE_STRIP_FOR_GAME_DIST(
			if (!Debug::Logger::Initialize())
			{
				throw ieException(TEXT("Failed to initialize core logger"), ieException::EExceptionCategory::EC_Engine);
			}
		)
	}

	Application::~Application()
	{
	}

	void Application::DumpApp()
	{
		Debug::Logger::AppendMessageForCoreDump(TEXT("Core dump requested by application."));
		Debug::Logger::InitiateCoreDump();
	}

	void Application::Initialize()
	{
		ScopedMilliSecondTimer(TEXT("Core app init"));

#if IE_RENDER_MULTI_PLATFORM
			using namespace Graphics;
			IRenderContext* pRenderContext = NULL;
		// TEST
		{
			ERenderBackend api = ERenderBackend::Direct3D_12;
			// Setup renderer
			{
				switch (api)
				{
				case ERenderBackend::Direct3D_12:
				{
					IE_LOG(Log, TEXT("Initializing graphics context with D3D12 rendering backend."));
					DX12::D3D12RenderContextFactory Factory;
					Factory.CreateContext(&pRenderContext, m_pWindow);
				}
				break;
				default:
					break;
				}
			}
			IE_LOG(Log, TEXT("Graphics context initialized."));

			// TODO: View and Scissor rects should be controlled elsewhere
			ViewPort ViewPort;
			ViewPort.TopLeftX = 0.f;
			ViewPort.TopLeftY = 0.f;
			ViewPort.MinDepth = 0.f;
			ViewPort.MaxDepth = 1.f;
			ViewPort.Width = (float)m_pWindow->GetWidth();
			ViewPort.Height = (float)m_pWindow->GetHeight();

			Rect ScissorRect;
			ScissorRect.Left = 0;
			ScissorRect.Top = 0;
			ScissorRect.Right = m_pWindow->GetWidth();
			ScissorRect.Bottom = m_pWindow->GetHeight();

			ISwapChain* pSwapChain = pRenderContext->GetSwapChain();
			Color ClearColor(0.f, .0f, 1.f);
			pSwapChain->SetClearColor(ClearColor);

			m_pWindow->SetWindowMode(EWindowMode::WM_Windowed);


			IColorBuffer* pSceneBuffer = new DX12::D3D12ColorBuffer();
			pSceneBuffer->Create(g_pDevice, TEXT("Scene Buffer"), m_pWindow->GetWidth(), m_pWindow->GetHeight(), 1u, pSwapChain->GetDesc().Format);
			
			IRootSignature* pRS = NULL;
			RootSignatureDesc RSDesc = {};
			RSDesc.Flags |= RSF_AllowInputAssemblerLayout;
			g_pDevice->CreateRootSignature(RSDesc, &pRS);
			
			InputElementDesc InputElements[] =
			{
				{ "POSITION",	0, F_R32G32B32_FLOAT,		0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
				{ "COLOR",		0, F_R32G32B32A32_FLOAT,	0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
				{ "TEXCOORD",	0, F_R32G32_FLOAT,			0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
			};
			
			::Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
			::Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;
			::Microsoft::WRL::ComPtr<ID3DBlob> errBuffer;
			UINT compileFlags =
#if IE_DEBUG
				// Enable better shader debugging with the graphics debugging tools.
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
				0;
#endif
			std::string ShaderSource = 
				R"(
					struct PSInput
					{
						float4 position : SV_POSITION;
						float4 color : COLOR;
					};

					struct VSInput
					{
						float4 Position : POSITION;
						float4 Color : COLOR;
						float2 texCoords : TEXCOORD;
					};

					PSInput VSMain(VSInput Input)
					{
						PSInput Result;

						Result.position = Input.Position;
						Result.color = Input.Color;

						return Result;
					}

					float4 PSMain(PSInput Input) : SV_TARGET
					{
						return Input.color;
					}
				)";

			HRESULT hr = D3DCompile(ShaderSource.data(), ShaderSource.size() * sizeof(char), NULL, NULL, NULL, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &errBuffer);
			hr = D3DCompile(ShaderSource.data(), ShaderSource.size() * sizeof(char), NULL, NULL, NULL, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &errBuffer);


			IPipelineState* pPSO = NULL;
			PipelineStateDesc PSODesc = {};
			PSODesc.VertexShader = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
			PSODesc.PixelShader = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };
			PSODesc.InputLayout.NumElements = _countof(InputElements);
			PSODesc.InputLayout.pInputElementDescs = InputElements;
			PSODesc.pRootSignature = pRS;
			PSODesc.DepthStencilState = CommonStructHelpers::CDepthStencilStateDesc();
			PSODesc.BlendState = CommonStructHelpers::CBlendDesc();
			PSODesc.RasterizerDesc = CommonStructHelpers::CRasterizerDesc();
			PSODesc.SampleMask = UINT_MAX;
			PSODesc.PrimitiveTopologyType = PTT_Triangle;
			PSODesc.NumRenderTargets = pSwapChain->GetDesc().BufferCount;
			PSODesc.RTVFormats[0] = pSwapChain->GetDesc().Format;
			PSODesc.SampleDesc = { 1, 0 };
			g_pDevice->CreatePipelineState(PSODesc, &pPSO);

			struct Mesh
			{
				Mesh()
				{
					m_DrawArgs.VertexBufferHandle = g_pGeometryManager->AllocateVertexBuffer();
					m_DrawArgs.IndexBufferHandle = g_pGeometryManager->AllocateIndexBuffer();
				}
				virtual ~Mesh()
				{
					UnInit();
				}
				void Load(const TChar* Filepath) { Init(); }

				void Draw(ICommandContext& GfxContext)
				{
					IVertexBuffer& VertBuffer = g_pGeometryManager->GetVertexBufferByUID(m_DrawArgs.VertexBufferHandle);
					IIndexBuffer& IndexBuffer = g_pGeometryManager->GetIndexBufferByUID(m_DrawArgs.IndexBufferHandle);

					GfxContext.SetPrimitiveTopologyType(PT_TiangleList);
					GfxContext.BindVertexBuffer(0, VertBuffer);
					GfxContext.BindIndexBuffer(IndexBuffer);
					GfxContext.DrawIndexedInstanced(m_DrawArgs.NumIndices, 1, 0, 0, 0);
				}

			protected:
				struct ScreenSpaceVertex
				{
					FVector3 Position;
					FVector4 Color;
					FVector2 TexCoords;
				};
				struct DrawArgs
				{
					UInt32 NumVerts;
					UInt32 NumIndices;
					VertexBufferUID VertexBufferHandle;
					IndexBufferUID IndexBufferHandle;
				};
				void Init()
				{
					ScreenSpaceVertex Verts[4] =
					{
						// Top Left
						{ { -0.25f, 0.0f, 0.0f }, { 1.0f, 0.5f, 0.0f, 1.0f } },
						// Top Right
						{ {  0.25f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
						// Bottom Left
						{ { -0.25f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
						// Bottom Right
						{ { 0.25f, -0.5f, 0.0f }, { 0.5f, 0.0f, 1.0f, 1.0f } }
					};
					const UInt32 VertexBufferSize = sizeof(Verts);
					m_DrawArgs.NumVerts = VertexBufferSize / sizeof(ScreenSpaceVertex);

					// Init Vertex buffer.
					IE_ASSERT(m_DrawArgs.VertexBufferHandle != IE_INVALID_VERTEX_BUFFER_HANDLE); // Vertex buffer was not registered properly with geometry buffer manager
					IVertexBuffer& Buffer = g_pGeometryManager->GetVertexBufferByUID(m_DrawArgs.VertexBufferHandle);
					Buffer.Create(TEXT("Vertex Buffer"), VertexBufferSize, sizeof(ScreenSpaceVertex), Verts);

					UInt32 Indices[6] =
					{
						0, 1, 3,
						0, 3, 2,
					};
					UInt32 IndexBufferSize = sizeof(Indices);
					m_DrawArgs.NumIndices = IndexBufferSize / sizeof(UInt32);

					// Init Index buffer
					IE_ASSERT(m_DrawArgs.IndexBufferHandle != IE_INVALID_INDEX_BUFFER_HANDLE); // Index buffer was not registered properly with geometry buffer manager
					IIndexBuffer& IndexBuffer = g_pGeometryManager->GetIndexBufferByUID(m_DrawArgs.IndexBufferHandle);
					IndexBuffer.Create(TEXT("Index Buffer"), IndexBufferSize, Indices);
				}

				void UnInit()
				{
					g_pGeometryManager->DeAllocateVertexBuffer(m_DrawArgs.VertexBufferHandle);
					g_pGeometryManager->DeAllocateIndexBuffer(m_DrawArgs.IndexBufferHandle);
				}

				DrawArgs m_DrawArgs;
			};


			Mesh mesh;
			mesh.Load(TEXT("TODO: Filepath"));

			while (m_Running)
			{
				// Process the window's Messages 
				m_pWindow->OnUpdate();

				pRenderContext->PreFrame();

				// Render stuff
				ICommandContext& CmdContext = ICommandContext::Begin(L"Frame");
				{
					IColorBuffer* pSwapChainBackBuffer = pSwapChain->GetColorBufferForCurrentFrame();
					IGPUResource& BackSwapChainBuffer = *DCast<IGPUResource*>(pSwapChainBackBuffer);
					CmdContext.TransitionResource(BackSwapChainBuffer, RS_RenderTarget);

					//const IColorBuffer* RTs[] = { pSceneBuffer };
					//Context.OMSetRenderTargets(1, RTs);
					//Context.ClearColorBuffer(*pSceneBuffer, ScissorRect);

					CmdContext.ClearColorBuffer(*pSwapChainBackBuffer, ScissorRect);
					const IColorBuffer* RTs[] = { pSwapChainBackBuffer };
					CmdContext.OMSetRenderTargets(1, RTs);
					CmdContext.RSSetViewPorts(1, &ViewPort);
					CmdContext.RSSetScissorRects(1, &ScissorRect);

					CmdContext.SetPipelineState(*pPSO);
					CmdContext.SetGraphicsRootSignature(*pRS);

					// TODO: Render model
					mesh.Draw(CmdContext);

					CmdContext.TransitionResource(BackSwapChainBuffer, RS_Present);
				}
				CmdContext.Finish();

				pRenderContext->SubmitFrame();
				pRenderContext->Present();
			}
		}
			SAFE_DELETE_PTR(pRenderContext);

		exit(EXIT_SUCCESS); // Just for easier debugging quit the entire program.
#endif

		// Initize the main file system.
		FileSystem::Init();

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

		// Push core app layers to the layer stack
		PushCoreLayers();
	}

	void Application::PostInit()
	{
		Renderer::PostInit();

		m_pWindow->PostInit();
		ResourceManager::Get().PostAppInit();
		m_pGameLayer->PostInit();

		IE_LOG(Verbose, TEXT("App Initialized"));
	}

	static bool s_ReloadRuntime = false;
	float g_GPUThreadFPS = 0.0f;
	void Application::RenderThread()
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
#if EDITOR_UI_ENABLED
			IE_STRIP_FOR_GAME_DIST
			(
				m_pImGuiLayer->Begin();
			for (Layer* pLayer : m_LayerStack)
				pLayer->OnImGuiRender();
			m_pGameLayer->OnImGuiRender();
			Renderer::OnEditorRender();
			m_pImGuiLayer->End();
			);
#endif

			// Submit for draw and present. 
			Renderer::ExecuteDraw();
			Renderer::SwapBuffers();
		}
	}

	Application::EErrorCode Application::Run()
	{
		IE_ADD_FOR_GAME_DIST(
			BeginPlay(AppBeginPlayEvent{})
		);

		// Put all rendering on another thread. 
		std::thread RenderThread(&Application::RenderThread, this);

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

		// Shutdown the application and release all resources.
		Shutdown();

		return EC_Success;
	}

	Application::EErrorCode Application::RunSingleThreaded()
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
#if EDITOR_UI_ENABLED
				IE_STRIP_FOR_GAME_DIST
				(
					m_pImGuiLayer->Begin();
				for (Layer* pLayer : m_LayerStack)
					pLayer->OnImGuiRender();
				m_pGameLayer->OnImGuiRender();
				m_pImGuiLayer->End();
				);
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

	void Application::Shutdown()
	{
		Renderer::Destroy();
		m_pWindow->Shutdown();
	}

	void Application::PushCoreLayers()
	{
#if IE_PLATFORM_BUILD_WIN32 && (EDITOR_UI_ENABLED)
		switch (Renderer::GetAPI())
		{
		case Renderer::ETargetRenderAPI::Direct3D_11:
			IE_STRIP_FOR_GAME_DIST(
				m_pImGuiLayer = new D3D11ImGuiLayer();
			PushOverlay(m_pImGuiLayer);
			);
			break;
		case Renderer::ETargetRenderAPI::Direct3D_12:
			IE_STRIP_FOR_GAME_DIST(
				m_pImGuiLayer = new D3D12ImGuiLayer();
			PushOverlay(m_pImGuiLayer);
			);
			break;
		default:
			IE_LOG(Error, TEXT("Failed to create ImGui layer in application with API of type \"%i\" Or application has disabled editor."), Renderer::GetAPI());
			break;
		}
#endif

		IE_STRIP_FOR_GAME_DIST(
			m_pEditorLayer = new EditorLayer();
		PushOverlay(m_pEditorLayer);
		)

			m_pPerfOverlay = new PerfOverlay();
		PushOverlay(m_pPerfOverlay);
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverLay(layer);
		layer->OnAttach();
	}



	// -----------------
	// Events Callbacks |
	// -----------------

	void Application::OnEvent(Event& e)
	{
		EventDispatcher Dispatcher(e);
		Dispatcher.Dispatch<WindowCloseEvent>(IE_BIND_LOCAL_EVENT_FN(Application::OnWindowClose));
		Dispatcher.Dispatch<WindowResizeEvent>(IE_BIND_LOCAL_EVENT_FN(Application::OnWindowResize));
		Dispatcher.Dispatch<WindowToggleFullScreenEvent>(IE_BIND_LOCAL_EVENT_FN(Application::OnWindowFullScreen));
		Dispatcher.Dispatch<SceneSaveEvent>(IE_BIND_LOCAL_EVENT_FN(Application::SaveScene));
		Dispatcher.Dispatch<AppBeginPlayEvent>(IE_BIND_LOCAL_EVENT_FN(Application::BeginPlay));
		Dispatcher.Dispatch<AppEndPlayEvent>(IE_BIND_LOCAL_EVENT_FN(Application::EndPlay));
		Dispatcher.Dispatch<AppScriptReloadEvent>(IE_BIND_LOCAL_EVENT_FN(Application::ReloadScripts));
		Dispatcher.Dispatch<ShaderReloadEvent>(IE_BIND_LOCAL_EVENT_FN(Application::ReloadShaders));

		// Process input event callbacks. 
		m_InputDispatcher.ProcessInputEvent(e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		m_pWindow->Resize(e.GetWidth(), e.GetHeight(), e.GetIsMinimized());
#if !IE_RENDER_MULTI_PLATFORM
		Renderer::PushEvent<WindowResizeEvent>(e);
#endif

		return true;
	}

	bool Application::OnWindowFullScreen(WindowToggleFullScreenEvent& e)
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

	bool Application::OnAppSuspendingEvent(AppSuspendingEvent& e)
	{
		m_IsSuspended = true;
		return true;
	}

	bool Application::OnAppResumingEvent(AppResumingEvent& e)
	{
		m_IsSuspended = false;
		return true;
	}

	bool Application::SaveScene(SceneSaveEvent& e)
	{
		std::future<bool> Future = std::async(std::launch::async, FileSystem::WriteSceneToJson, m_pGameLayer->GetScene());
		return true;
	}

	bool Application::BeginPlay(AppBeginPlayEvent& e)
	{
		PushLayer(m_pGameLayer);
		return true;
	}

	bool Application::EndPlay(AppEndPlayEvent& e)
	{
		m_pGameLayer->EndPlay();
		m_LayerStack.PopLayer(m_pGameLayer);
		m_pGameLayer->OnDetach();
		return true;
	}

	bool Application::ReloadScripts(AppScriptReloadEvent& e)
	{
		IE_LOG(Log, TEXT("Reloading C# Scripts"));
#if IE_PLATFORM_BUILD_WIN32
		ResourceManager::Get().GetMonoScriptManager().ReCompile();
#endif
		return true;
	}

	bool Application::ReloadShaders(ShaderReloadEvent& e)
	{
		//Renderer::OnShaderReload(); 
		Renderer::PushEvent<ShaderReloadEvent>(e);
		return true;
	}
}
