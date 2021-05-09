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
#include "Runtime/Graphics/Public/ResourceManagement/IConstantBufferManager.h"
#endif // IE_RENDER_MULTI_PLATFORM


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
			Color ClearColor(0.f, .3f, .3f);
			pSwapChain->SetClearColor(ClearColor);

			m_pWindow->SetWindowMode(EWindowMode::WM_Windowed);


			IColorBuffer* pSceneBuffer = NULL;
			g_pDevice->CreateColorBuffer(TEXT("Scene Buffer"), m_pWindow->GetWidth(), m_pWindow->GetHeight(), 1u, pSwapChain->GetDesc().Format, &pSceneBuffer);

			IDepthBuffer* pDepthBuffer = NULL;
			g_pDevice->CreateDepthBuffer(TEXT("Scene Depth Buffer"), m_pWindow->GetWidth(), m_pWindow->GetHeight(), F_D32_Float, &pDepthBuffer);

			enum EShaderBufferRegisters
			{
				SBR_SceneConstants = 0,
				SBR_MeshWorld = 1,
				SBR_MaterialParams = 2,
				SBR_Texture = 3,
			};

			RootParameter pRootParams[4];
			ZeroMem(pRootParams, sizeof(RootParameter) * _countof(pRootParams));
			// Scene Constants
			pRootParams[0].ShaderVisibility = SV_All;
			pRootParams[0].ParameterType = RPT_ConstantBufferView;
			pRootParams[0].Descriptor.ShaderRegister = SBR_SceneConstants;
			pRootParams[0].Descriptor.RegisterSpace = 0;
			// Mesh World
			pRootParams[1].ShaderVisibility = SV_All;
			pRootParams[1].ParameterType = RPT_ConstantBufferView;
			pRootParams[1].Descriptor.ShaderRegister = SBR_MeshWorld;
			pRootParams[1].Descriptor.RegisterSpace = 0;
			// Material
			pRootParams[2].ShaderVisibility = SV_All;
			pRootParams[2].ParameterType = RPT_ConstantBufferView;
			pRootParams[2].Descriptor.ShaderRegister = SBR_MaterialParams;
			pRootParams[2].Descriptor.RegisterSpace = 0;
			// Texture
			DescriptorRange pDescriptorRanges[1];
				ZeroMem(pDescriptorRanges, sizeof(RootDescriptor) * _countof(pDescriptorRanges));
				pDescriptorRanges[0].Type = DRT_ShaderResourceView;
				pDescriptorRanges[0].BaseShaderRegister = 0;
				pDescriptorRanges[0].NumDescriptors = 1;
				pDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
				pDescriptorRanges[0].RegisterSpace = 0;
			pRootParams[3].ShaderVisibility = SV_Pixel;
			pRootParams[3].ParameterType = RPT_DescriptorTable;
			pRootParams[3].DescriptorTable.NumDescriptors = _countof(pDescriptorRanges);
			pRootParams[3].DescriptorTable.pDescriptorRanges = pDescriptorRanges;

			constexpr float MinLOD = 0.0f, MaxLOD = 9.0f;
			StaticSamplerDesc pSamplers[1];
			pSamplers[0].ShaderRegister = 0;
			pSamplers[0].Filter = F_Anisotropic;
			pSamplers[0].AddressU = TAM_Wrap;
			pSamplers[0].AddressV = TAM_Wrap;
			pSamplers[0].AddressW = TAM_Wrap;
			pSamplers[0].MipLODBias = 0;
			pSamplers[0].MaxAnisotropy = 1;
			pSamplers[0].ComparisonFunc = CF_LessEqual;
			pSamplers[0].BorderColor = SBC_Opaque_White;
			pSamplers[0].MinLOD = MinLOD;
			pSamplers[0].MaxLOD = MaxLOD;
			pSamplers[0].ShaderVisibility = SV_Pixel;
			pSamplers[0].RegisterSpace = 0;


			IRootSignature* pRS = NULL;
			RootSignatureDesc RSDesc = {};
			RSDesc.Flags |= RSF_AllowInputAssemblerLayout;
			RSDesc.pParameters = pRootParams;
			RSDesc.NumParams = _countof(pRootParams);
			RSDesc.pStaticSamplers = pSamplers;
			RSDesc.NumStaticSamplers = _countof(pSamplers);
			g_pDevice->CreateRootSignature(RSDesc, &pRS);

			InputElementDesc InputElements[] =
			{
				{ "POSITION",	0, F_R32G32B32_Float,		0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
				{ "COLOR",		0, F_R32G32B32A32_Float,	0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
				{ "UVs",		0, F_R32G32_Float,			0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
			};

			::Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
			::Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;
			UInt32 compileFlags =
#if IE_DEBUG
				// Enable better shader debugging with the graphics debugging tools.
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
				0;
#endif
			std::string ShaderSource =
R"(
			// Constant Buffers
			cbuffer SceneConstants : register(b0)
			{
				float4x4 ViewMat;
				float4x4 ProjMat;
				float WorldTime;
			};
			cbuffer MeshWorld : register(b1)
			{
				float4x4 WorldMat;
			}
			cbuffer Material : register(b2)
			{
				float4 Color;
			}

			// Textures and Samplers
			Texture2D Texture : register(t0);
			SamplerState LinearWrapSampler : register(s0);

			// Structs 
			struct VSInput
			{
				float3 Position : POSITION;
				float4 Color : COLOR;
				float2 UVs	: UVs;
			};
			struct PSInput
			{
				float4 Position : SV_POSITION;
				float4 VertexColor : COLOR;
				float2 UVs : UVs;
			};

			PSInput VSMain(VSInput Input)
			{
				PSInput Result;

				float4x4 WorldView              = mul(WorldMat, ViewMat);
				float4x4 worldViewProjection    = mul(WorldView, ProjMat);
    
				Result.Position = mul(float4(Input.Position, 1.0f), worldViewProjection);
				Result.VertexColor = Input.Color;
				Result.UVs = Input.UVs;

				return Result;
			}

			float4 PSMain(PSInput Input) : SV_TARGET
			{
				//return float4(Input.UVs.x, Input.UVs.y, 0, 1);
				float3 TexCol = Texture.Sample(LinearWrapSampler, Input.UVs).rgb;
				return float4(TexCol, 1.0f);

				float4 Result = lerp(Input.VertexColor, Color, 0.3f);
				return Result;	
			}
)";

			HRESULT hr = D3DCompile(ShaderSource.data(), ShaderSource.size() * sizeof(char), NULL, NULL, NULL, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, NULL);
			hr = D3DCompile(ShaderSource.data(), ShaderSource.size() * sizeof(char), NULL, NULL, NULL, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, NULL);


			IPipelineState* pScenePassPSO = NULL;
			PipelineStateDesc PSODesc = {};
			PSODesc.VertexShader = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
			PSODesc.PixelShader = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };
			PSODesc.InputLayout.NumElements = _countof(InputElements);
			PSODesc.InputLayout.pInputElementDescs = InputElements;
			PSODesc.pRootSignature = pRS;
			PSODesc.DepthStencilState = CommonStructHelpers::CDepthStencilStateDesc();
			//PSODesc.DepthStencilState.DepthFunc = CF_GreaterEqual;
			PSODesc.BlendState = CommonStructHelpers::CBlendDesc();
			PSODesc.RasterizerDesc = CommonStructHelpers::CRasterizerDesc();
			PSODesc.SampleMask = UINT_MAX;
			PSODesc.PrimitiveTopologyType = PTT_Triangle;
			PSODesc.NumRenderTargets = pSwapChain->GetDesc().BufferCount;
			PSODesc.RTVFormats[0] = pSwapChain->GetDesc().Format;
			PSODesc.DSVFormat = DCast<IPixelBuffer*>(pDepthBuffer)->GetFormat();
			PSODesc.SampleDesc = { 1, 0 };
			g_pDevice->CreatePipelineState(PSODesc, &pScenePassPSO);



			__declspec(align(16)) struct SceneConstants
			{
				FMatrix ViewMat;
				FMatrix ProjMat;
				float WorldTime;
			};
			__declspec(align(16)) struct MeshWorld
			{
				FMatrix WorldMat;
			};
			__declspec(align(16)) struct Material
			{
				FVector4 Color;
			};
			Graphics::IConstantBuffer* pSceneConstantBuffer = NULL;
			g_pConstantBufferManager->CreateConstantBuffer(TEXT("Scene Constants"), &pSceneConstantBuffer, sizeof(SceneConstants));


			struct Mesh
			{
				Mesh()
					: m_pMaterialCB(NULL)
					, m_pMeshWorldCB(NULL)
				{
					Init();
				}
				virtual ~Mesh()
				{
					UnInit();
				}

				void Load(const EString& Path)
				{
					Vertex3D Verts[] = {
						// Front
						{ { -0.5f,  0.5f, -0.5f }, { 1.f, 0.f, 0.f, 1.f }, { 0.0f, 0.0f} },
						{ {  0.5f, -0.5f, -0.5f }, { 0.f, 1.f, 0.f, 1.f }, { 0.0f, 1.0f} },
						{ { -0.5f, -0.5f, -0.5f }, { 0.f, 0.f, 1.f, 1.f }, { 0.0f, 1.0f} },
						{ {  0.5f,  0.5f, -0.5f }, { 1.f, 0.f, 0.f, 1.f }, { 1.0f, 0.0f} },
						// Right 
						{ {  0.5f, -0.5f, -0.5f }, { 0.f, 1.f, 0.f, 1.f }, { 0.0f, 0.0f} },
						{ {  0.5f,  0.5f,  0.5f }, { 0.f, 0.f, 1.f, 1.f }, { 0.0f, 1.0f} },
						{ {  0.5f, -0.5f,  0.5f }, { 1.f, 0.f, 0.f, 1.f }, { 0.0f, 1.0f} },
						{ {  0.5f,  0.5f, -0.5f }, { 0.f, 1.f, 0.f, 1.f }, { 1.0f, 0.0f} },
						// Left
						{ { -0.5f,  0.5f,  0.5f }, { 0.f, 0.f, 1.f, 1.f }, { 0.0f, 0.0f } },
						{ { -0.5f, -0.5f, -0.5f }, { 1.f, 0.f, 0.f, 1.f }, { 0.0f, 1.0f } },
						{ { -0.5f, -0.5f,  0.5f }, { 0.f, 1.f, 0.f, 1.f }, { 0.0f, 1.0f } },
						{ { -0.5f,  0.5f, -0.5f }, { 0.f, 0.f, 1.f, 1.f }, { 1.0f, 0.0f } },
						// Back
						{ {  0.5f,  0.5f,  0.5f }, { 1.f, 0.f, 0.f, 1.f }, { 0.0f, 0.0f} },
						{ { -0.5f, -0.5f,  0.5f }, { 0.f, 1.f, 0.f, 1.f }, { 0.0f, 1.0f} },
						{ {  0.5f, -0.5f,  0.5f }, { 0.f, 0.f, 1.f, 1.f }, { 0.0f, 1.0f} },
						{ { -0.5f,  0.5f,  0.5f }, { 1.f, 0.f, 0.f, 1.f }, { 1.0f, 0.0f} },
						// Top
						{ { -0.5f,  0.5f, -0.5f }, { 0.f, 1.f, 0.f, 1.f }, { 0.0f, 0.0f} },
						{ {  0.5f,  0.5f,  0.5f }, { 0.f, 0.f, 1.f, 1.f }, { 0.0f, 1.0f} },
						{ {  0.5f,  0.5f, -0.5f }, { 1.f, 0.f, 0.f, 1.f }, { 0.0f, 1.0f} },
						{ { -0.5f,  0.5f,  0.5f }, { 0.f, 1.f, 0.f, 1.f }, { 1.0f, 0.0f} },
						// Bottom
						{ {  0.5f, -0.5f,  0.5f }, { 0.f, 0.f, 1.f, 1.f }, { 0.0f, 0.0f} },
						{ { -0.5f, -0.5f, -0.5f }, { 1.f, 0.f, 0.f, 1.f }, { 0.0f, 1.0f} },
						{ {  0.5f, -0.5f, -0.5f }, { 0.f, 1.f, 0.f, 1.f }, { 0.0f, 1.0f} },
						{ { -0.5f, -0.5f,  0.5f }, { 0.f, 0.f, 1.f, 1.f }, { 1.0f, 0.0f} },
					};

					const UInt32 VertexBufferSize = sizeof(Verts);
					m_DrawArgs.NumVerts = VertexBufferSize / sizeof(Vertex3D);

					// Init Vertex buffer.
					IE_ASSERT(m_DrawArgs.VertexBufferHandle != IE_INVALID_VERTEX_BUFFER_HANDLE); // Vertex buffer was not registered properly with geometry buffer manager.
					IVertexBuffer& Buffer = g_pGeometryManager->GetVertexBufferByUID(m_DrawArgs.VertexBufferHandle);
					Buffer.Create(TEXT("Vertex Buffer"), VertexBufferSize, sizeof(Vertex3D), Verts);

					UInt32 Indices[] =
					{
						// front face
						0, 1, 2, // first triangle
						0, 3, 1, // second triangle

						// left face
						4, 5, 6, // first triangle
						4, 7, 5, // second triangle

						// right face
						8, 9, 10, // first triangle
						8, 11, 9, // second triangle

						// back face
						12, 13, 14, // first triangle
						12, 15, 13, // second triangle

						// top face
						16, 17, 18, // first triangle
						16, 19, 17, // second triangle

						// bottom face
						20, 21, 22, // first triangle
						20, 23, 21, // second triangle
					};
					UInt32 IndexBufferSize = sizeof(Indices);
					m_DrawArgs.NumIndices = IndexBufferSize / sizeof(UInt32);

					// Init Index buffer
					IE_ASSERT(m_DrawArgs.IndexBufferHandle != IE_INVALID_INDEX_BUFFER_HANDLE); // Index buffer was not registered properly with geometry buffer manager.
					IIndexBuffer& IndexBuffer = g_pGeometryManager->GetIndexBufferByUID(m_DrawArgs.IndexBufferHandle);
					IndexBuffer.Create(TEXT("Index Buffer"), IndexBufferSize, Indices);
				}

				void Draw(ICommandContext& GfxContext)
				{
					IVertexBuffer& VertBuffer = g_pGeometryManager->GetVertexBufferByUID(m_DrawArgs.VertexBufferHandle);
					IIndexBuffer& IndexBuffer = g_pGeometryManager->GetIndexBufferByUID(m_DrawArgs.IndexBufferHandle);

					// Set the material.
					Material* pMat = m_pMaterialCB->GetBufferPointer<Material>();
					pMat->Color = m_Material.Color;
					GfxContext.SetGraphicsConstantBuffer(SBR_MaterialParams, m_pMaterialCB);

					// Set the world buffer.
					MeshWorld* pWorld = m_pMeshWorldCB->GetBufferPointer<MeshWorld>();
					pWorld->WorldMat = m_Transform.GetWorldMatrix().Transpose();
					GfxContext.SetGraphicsConstantBuffer(SBR_MeshWorld, m_pMeshWorldCB);

					// Set material texture
					GfxContext.SetTexture(SBR_Texture, *pActiveTexture);

					GfxContext.SetPrimitiveTopologyType(PT_TiangleList);
					GfxContext.BindVertexBuffer(0, VertBuffer);
					GfxContext.BindIndexBuffer(IndexBuffer);
					GfxContext.DrawIndexedInstanced(m_DrawArgs.NumIndices, 1, 0, 0, 0);
				}

				inline Material& GetMaterial()		{ return m_Material; }
				inline ieTransform& GetTransform()	{ return m_Transform; }

				// TODO: Should be moved to a more expanded material class.
				ITextureRef m_AlbedoTexture;
				ITextureRef m_NormalTexture;
				ITextureRef* pActiveTexture;
			protected:
				struct Vertex3D
				{
					FVector3 Position;
					FVector4 Color;
					FVector2 UV0;
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
					m_DrawArgs.VertexBufferHandle = g_pGeometryManager->AllocateVertexBuffer();
					m_DrawArgs.IndexBufferHandle = g_pGeometryManager->AllocateIndexBuffer();

					g_pConstantBufferManager->CreateConstantBuffer(TEXT("Material Params"), &m_pMaterialCB, sizeof(Material));
					g_pConstantBufferManager->CreateConstantBuffer(TEXT("Mesh World Params"), &m_pMeshWorldCB, sizeof(MeshWorld));


					// TODO: Only dds textures can be loaded right now.
					const TChar* TexturePath = L"Content/Textures/Debug/DebugCheckerBoard_Albedo.dds";
					m_AlbedoTexture = g_pTextureManager->LoadTexture(TexturePath, DT_Magenta2D, false);
					const TChar* NormalTexturePath = L"Content/Textures/RustedIron/RustedIron_Albedo.dds";
					m_NormalTexture = g_pTextureManager->LoadTexture(NormalTexturePath, DT_Magenta2D, false);

					pActiveTexture = &m_AlbedoTexture;
				}

				void UnInit()
				{
					g_pGeometryManager->DeAllocateVertexBuffer(m_DrawArgs.VertexBufferHandle);
					g_pGeometryManager->DeAllocateIndexBuffer(m_DrawArgs.IndexBufferHandle);

					g_pConstantBufferManager->DestroyConstantBuffer(m_pMeshWorldCB->GetUID());
					g_pConstantBufferManager->DestroyConstantBuffer(m_pMaterialCB->GetUID());
				}

				Graphics::IConstantBuffer* m_pMaterialCB;
				Graphics::IConstantBuffer* m_pMeshWorldCB;
				ieTransform m_Transform;
				Material m_Material;
				DrawArgs m_DrawArgs;

			};

			struct Actor
			{
				using Super = Actor;
				Actor()
				{
				}
				~Actor() {}

				virtual void Update(float DeltaMs)
				{
				}

				virtual void Render(ICommandContext& RenderContext)
				{
				}

				ieTransform m_Transform;
			};

			struct StaticMeshActor : public Actor
			{
				StaticMeshActor()
				{
					m_Mesh.Load(L"Path");
					m_Mesh.GetTransform().SetParent(&m_Transform);
				}

				virtual void Update(float DeltaMs) override
				{
					Super::Update(DeltaMs);

					FVector3 Rotation = m_Transform.GetRotation();
					m_Mesh.GetMaterial().Color = FVector4(Rotation.x, Rotation.y, Rotation.z, 1.f);
					m_Mesh.GetMaterial().Color /= 2.f;


					static float Timer = 0.f;
					static float Time = 2.f;
					Timer += DeltaMs;
					if (Timer >= Time)
					{
						Timer = 0.f;
						static bool IsFirstTex = false;
						if (IsFirstTex) 
						{
							IsFirstTex = false;
							m_Mesh.pActiveTexture = &m_Mesh.m_NormalTexture;
						}
						else
						{
							IsFirstTex = true;
							m_Mesh.pActiveTexture = &m_Mesh.m_AlbedoTexture;
						}
					}

				}

				virtual void Render(ICommandContext& RenderContext) override
				{
					Super::Render(RenderContext);
					m_Mesh.Draw(RenderContext);
				}

				Mesh m_Mesh;
			};

			struct Camera : public Actor
			{
				Camera(FVector2 ViewPortDims)
				{
					UpdateViewMat();
					SetProjectionValues(45.f, ViewPortDims.x / ViewPortDims.y, 0.1f, 1000.f);
				}
				virtual ~Camera() {}

				void Update(float DeltaMs) override
				{
					Super::Update(DeltaMs);

					UpdateViewMat();
				}

				float m_NearZ;
				float m_FarZ;
				FMatrix m_ViewMat;
				FMatrix m_ProjMat;

			private:
				void SetProjectionValues(float FOVDegrees, float AspectRatio, float NearZ, float FarZ)
				{
					m_NearZ = NearZ;
					m_FarZ = FarZ;
					float fovRadians = FOVDegrees * (3.14f / 180.0f);
					m_ProjMat = XMMatrixPerspectiveFovLH(fovRadians, AspectRatio, NearZ, FarZ);
				}

				void UpdateViewMat()
				{
					FVector3 Target = m_Transform.GetPosition() + m_Transform.GetLocalForward();
					m_ViewMat = XMMatrixLookAtLH(m_Transform.GetPosition(), Target, m_Transform.GetLocalUp());
				}
			};

			Camera Camera(m_pWindow->GetDimensions());
			Camera.m_Transform.SetPosition(0.f, 0.f, -5.f);
			StaticMeshActor MyCubeActor;
			StaticMeshActor AnotherCubeActor;
			AnotherCubeActor.m_Transform.SetPosition(2.f, 0.f, 0.f);
			AnotherCubeActor.m_Transform.SetParent(&MyCubeActor.m_Transform);
			

			FrameTimer GFXTimer;

			while (m_Running)
			{
				GFXTimer.Tick();
				float DeltaMs = GFXTimer.DeltaTime();


				// Process the window's Messages 
				m_pWindow->OnUpdate();

				pRenderContext->PreFrame();

				// Render stuff
				ICommandContext& CmdContext = ICommandContext::Begin(L"Frame");
				{
					// Transition
					IColorBuffer* pSwapChainBackBuffer = pSwapChain->GetColorBufferForCurrentFrame();
					IGPUResource& BackSwapChainBuffer = *DCast<IGPUResource*>(pSwapChainBackBuffer);
					CmdContext.TransitionResource(BackSwapChainBuffer, RS_RenderTarget);
					IGPUResource& DepthBufferResource = *DCast<IGPUResource*>(pDepthBuffer);
					CmdContext.TransitionResource(DepthBufferResource, RS_DepthWrite);

					// TODO: For PostFX
					//const IColorBuffer* RTs[] = { pSceneBuffer };
					//CmdContext.OMSetRenderTargets(1, RTs);
					//CmdContext.ClearColorBuffer(*pSceneBuffer, ScissorRect);

					// Bind
					CmdContext.SetDescriptorHeap(RHT_CBV_SRV_UAV, g_pTextureHeap);

					// Clear
					CmdContext.ClearColorBuffer(*pSwapChainBackBuffer, ScissorRect);
					CmdContext.ClearDepth(*pDepthBuffer);

					// Set
					const IColorBuffer* RTs[] = { pSwapChainBackBuffer };
					CmdContext.OMSetRenderTargets(1, RTs, pDepthBuffer);
					CmdContext.RSSetViewPorts(1, &ViewPort);
					CmdContext.RSSetScissorRects(1, &ScissorRect);
					CmdContext.SetPipelineState(*pScenePassPSO);
					CmdContext.SetGraphicsRootSignature(*pRS);


					// Update
					Camera.Update(DeltaMs);
					MyCubeActor.Update(DeltaMs);
					AnotherCubeActor.Update(DeltaMs);
					MyCubeActor.m_Transform.Rotate(0.001f, 0.002f, 0.003f);
					{
						// Set Constant Buffers
						SceneConstants* pData = pSceneConstantBuffer->GetBufferPointer<SceneConstants>();
						pData->ViewMat = Camera.m_ViewMat.Transpose();
						pData->ProjMat = Camera.m_ProjMat.Transpose();
						pData->WorldTime = (float)GFXTimer.Seconds();
						CmdContext.SetGraphicsConstantBuffer(SBR_SceneConstants, pSceneConstantBuffer);
					}

					// Draw
					MyCubeActor.Render(CmdContext);
					AnotherCubeActor.Render(CmdContext);

					// Present
					CmdContext.TransitionResource(BackSwapChainBuffer, RS_Present);
				}
				CmdContext.Finish();

				pRenderContext->SubmitFrame();
				pRenderContext->Present();

				IE_LOG(Log, TEXT("FPS: %f"), GFXTimer.FPS());
			}
		}

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
