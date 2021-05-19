// Copyright Insight Interactive. All Rights Reserved.
#include <Engine_pch.h>

#include "Runtime/Graphics/Public/WorldRenderer/WorldRenderer.h"

#include "Runtime/Core/Public/ieObject/Components/ieCameraComponent.h"
#include "Runtime/Core/Public/ieObject/ieWorld.h"

//#ifdef IE_WITH_D3D12
#include "Platform/DirectX12/Public/D3D12RenderContextFactory.h"
#include <d3dcompiler.h>
//#endif

namespace Insight
{

	void WorldRenderer::Initialize(std::shared_ptr<Window> pWindow, Graphics::ERenderBackend api)
	{
		IE_ASSERT(pWindow.get() != NULL); // Trying to create a renderer with no window to render to!
		m_pWindow = pWindow;


		// Setup renderer
		switch (api)
		{
		case Graphics::RB_Direct3D12:
		{
			IE_LOG(Log, TEXT("Initializing graphics context with D3D12 rendering backend."));

			ScopedMilliSecondTimer(TEXT("Render backend init"));
			Graphics::DX12::D3D12RenderContextFactory Factory;
			Factory.CreateContext(&m_pRenderContext, pWindow);
		}
		break;
		default:
			break;
		}

		ScopedMilliSecondTimer(TEXT("Render context resource create"));

		// TODO: View and Scissor rects should be controlled elsewhere
		m_SceneViewPort.TopLeftX = 0.f;
		m_SceneViewPort.TopLeftY = 0.f;
		m_SceneViewPort.MinDepth = 0.f;
		m_SceneViewPort.MaxDepth = 1.f;
		m_SceneViewPort.Width = (float)m_pWindow->GetWidth();
		m_SceneViewPort.Height = (float)m_pWindow->GetHeight();

		m_SceneScissorRect.Left = 0;
		m_SceneScissorRect.Top = 0;
		m_SceneScissorRect.Right = m_pWindow->GetWidth();
		m_SceneScissorRect.Bottom = m_pWindow->GetHeight();

		m_pSwapChain = m_pRenderContext->GetSwapChain();
		Color ClearColor(0.f, .3f, .3f);
		m_pSwapChain->SetClearColor(ClearColor);

		// TODO: Set from somewhere else.
		m_pWindow->SetWindowMode(EWindowMode::WM_Windowed);

		CreateResources();

		IE_LOG(Log, TEXT("Graphics context initialized."));
	}


	void WorldRenderer::CreateResources()
	{
		Graphics::g_pDevice->CreateColorBuffer(TEXT("Scene Buffer"), m_pWindow->GetWidth(), m_pWindow->GetHeight(), 1u, m_pSwapChain->GetDesc().Format, &m_pSceneBuffer);

		Graphics::g_pDevice->CreateDepthBuffer(TEXT("Scene Depth Buffer"), m_pWindow->GetWidth(), m_pWindow->GetHeight(), Graphics::F_D32_Float, &m_pDepthBuffer);


		Graphics::RootParameter pRootParams[6];
		ZeroMem(pRootParams, sizeof(Graphics::RootParameter) * _countof(pRootParams));
		// Scene Constants
		pRootParams[0].ShaderVisibility = Graphics::SV_All;
		pRootParams[0].ParameterType = Graphics::RPT_ConstantBufferView;
		pRootParams[0].Descriptor.ShaderRegister = SPI_SceneConstants;
		pRootParams[0].Descriptor.RegisterSpace = 0;
		// Mesh World
		pRootParams[1].ShaderVisibility = Graphics::SV_All;
		pRootParams[1].ParameterType = Graphics::RPT_ConstantBufferView;
		pRootParams[1].Descriptor.ShaderRegister = SPI_MeshWorld;
		pRootParams[1].Descriptor.RegisterSpace = 0;
		// Material
		pRootParams[2].ShaderVisibility = Graphics::SV_All;
		pRootParams[2].ParameterType = Graphics::RPT_ConstantBufferView;
		pRootParams[2].Descriptor.ShaderRegister = SPI_MaterialParams;
		pRootParams[2].Descriptor.RegisterSpace = 0;
		// Lights
		pRootParams[3].ShaderVisibility = Graphics::SV_All;
		pRootParams[3].ParameterType = Graphics::RPT_ConstantBufferView;
		pRootParams[3].Descriptor.ShaderRegister = SPI_Lights;
		pRootParams[3].Descriptor.RegisterSpace = 0;
		// Textures
		// Albedo
		Graphics::DescriptorRange pDescriptorRanges[2];
		ZeroMem(pDescriptorRanges, sizeof(Graphics::RootDescriptor) * _countof(pDescriptorRanges));
		pDescriptorRanges[0].Type = Graphics::DRT_ShaderResourceView;
		pDescriptorRanges[0].BaseShaderRegister = 0;
		pDescriptorRanges[0].NumDescriptors = 1;
		pDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		pDescriptorRanges[0].RegisterSpace = 0;
		pDescriptorRanges[1].Type = Graphics::DRT_ShaderResourceView;
		pDescriptorRanges[1].BaseShaderRegister = 1;
		pDescriptorRanges[1].NumDescriptors = 1;
		pDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		pDescriptorRanges[1].RegisterSpace = 0;
		pRootParams[4].ShaderVisibility = Graphics::SV_Pixel;
		pRootParams[4].ParameterType = Graphics::RPT_DescriptorTable;
		pRootParams[4].DescriptorTable.NumDescriptors = 1;
		pRootParams[4].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[0];
		// Normal
		pRootParams[5].ShaderVisibility = Graphics::SV_Pixel;
		pRootParams[5].ParameterType = Graphics::RPT_DescriptorTable;
		pRootParams[5].DescriptorTable.NumDescriptors = 1;
		pRootParams[5].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[1];


		constexpr float MinLOD = 0.0f, MaxLOD = 9.0f;
		Graphics::StaticSamplerDesc pSamplers[1];
		pSamplers[0].ShaderRegister = 0;
		pSamplers[0].Filter = Graphics::F_Anisotropic;
		pSamplers[0].AddressU = Graphics::TAM_Wrap;
		pSamplers[0].AddressV = Graphics::TAM_Wrap;
		pSamplers[0].AddressW = Graphics::TAM_Wrap;
		pSamplers[0].MipLODBias = 0;
		pSamplers[0].MaxAnisotropy = 1;
		pSamplers[0].ComparisonFunc = Graphics::CF_LessEqual;
		pSamplers[0].BorderColor = Graphics::SBC_Opaque_White;
		pSamplers[0].MinLOD = MinLOD;
		pSamplers[0].MaxLOD = MaxLOD;
		pSamplers[0].ShaderVisibility = Graphics::SV_Pixel;
		pSamplers[0].RegisterSpace = 0;


		Graphics::RootSignatureDesc RSDesc = {};
		RSDesc.Flags |= Graphics::RSF_AllowInputAssemblerLayout;
		RSDesc.pParameters = pRootParams;
		RSDesc.NumParams = _countof(pRootParams);
		RSDesc.pStaticSamplers = pSamplers;
		RSDesc.NumStaticSamplers = _countof(pSamplers);
		Graphics::g_pDevice->CreateRootSignature(RSDesc, &m_pCommonRS);

		//
		// Create pipeline
		//

		Graphics::InputElementDesc InputElements[] =
		{
			{ "POSITION",	0, Graphics::F_R32G32B32_Float,		0, IE_APPEND_ALIGNED_ELEMENT,		Graphics::IC_PerVertexData, 0 },
			{ "NORMAL",		0, Graphics::F_R32G32B32_Float,		0, IE_APPEND_ALIGNED_ELEMENT,		Graphics::IC_PerVertexData, 0 },
			{ "TANGENT",		0, Graphics::F_R32G32B32_Float,		0, IE_APPEND_ALIGNED_ELEMENT,		Graphics::IC_PerVertexData, 0 },
			{ "BITANGENT",		0, Graphics::F_R32G32B32_Float,		0, IE_APPEND_ALIGNED_ELEMENT,		Graphics::IC_PerVertexData, 0 },
			{ "COLOR",		0, Graphics::F_R32G32B32A32_Float,	0, IE_APPEND_ALIGNED_ELEMENT,		Graphics::IC_PerVertexData, 0 },
			{ "UVs",		0, Graphics::F_R32G32_Float,			0, IE_APPEND_ALIGNED_ELEMENT,	Graphics::IC_PerVertexData, 0 },
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
	float3 CameraPos;
	float3 ViewVector;
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
struct PointLight
{
	float3 Position;
	float3 Color;
	float Brightness;
};
cbuffer SceneLights : register(b3)
{
	PointLight PointLights[4];
}

// Textures and Samplers
Texture2D Albedo : register(t0);
Texture2D Normal : register(t1);
SamplerState LinearWrapSampler : register(s0);

// Structs 
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 BiTangent : BITANGENT;
	float4 Color : COLOR;
	float2 UVs	: UVs;
};
struct PSInput
{
	float4 Position : SV_POSITION;
	float3 WorldPos : WORLDPOS;
	float4 VertexColor : COLOR;
	float2 UVs : UVs;
};

PSInput VSMain(VSInput Input)
{
	PSInput Result;

	float4x4 WorldView              = mul(WorldMat, ViewMat);
	float4x4 worldViewProjection    = mul(WorldView, ProjMat);
    
	Result.Position = mul(float4(Input.Position, 1.0f), worldViewProjection);
	Result.WorldPos = mul(float4(Input.Position, 1.0f), WorldMat).xyz;
	Result.VertexColor = Input.Color;
	Result.UVs = Input.UVs;

	return Result;
}

float4 PSMain(PSInput Input) : SV_TARGET
{
	float3 AlbedoSample = Albedo.Sample(LinearWrapSampler, Input.UVs).rgb;
	float3 NormalSample = Normal.Sample(LinearWrapSampler, Input.UVs).rgb;
	float3 NormalVec = normalize(NormalSample);

	float3 LightDir = -normalize(PointLights[0].Position - Input.WorldPos);
	float Angle = max(dot(NormalVec, LightDir), 0);

	float Distance = length(PointLights[0].Position - Input.WorldPos);
	float Attenuation = 1.0f / (Distance * Distance);
	float3 Radiance = (PointLights[0].Color * PointLights[0].Brightness) * Attenuation;

	float3 Color = AlbedoSample * Radiance;
	float3 Result = Color ;

//return float4(PointLights[0].Position - Input.WorldPos, 1.0);
	return float4(Result, 1.0f);
}
)";
		ID3DBlob* pError = NULL;
		HRESULT hr = D3DCompile(ShaderSource.data(), ShaderSource.size() * sizeof(char), NULL, NULL, NULL, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &pError);
		if (pError != NULL)
		{
			std::string err = (char*)pError->GetBufferPointer();
			IE_LOG(Error, TEXT("Shader Compile Error: %s"), StringHelper::StringToWide(err).c_str());
		}
		hr = D3DCompile(ShaderSource.data(), ShaderSource.size() * sizeof(char), NULL, NULL, NULL, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &pError);
		if (pError != NULL)
		{
			std::string err = (char*)pError->GetBufferPointer();
			IE_LOG(Error, TEXT("Shader Compile Error: %s"), StringHelper::StringToWide(err).c_str());
		}

		Graphics::PipelineStateDesc PSODesc = {};
		PSODesc.VertexShader = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
		PSODesc.PixelShader = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };
		PSODesc.InputLayout.NumElements = _countof(InputElements);
		PSODesc.InputLayout.pInputElementDescs = InputElements;
		PSODesc.pRootSignature = m_pCommonRS;
		PSODesc.DepthStencilState = Graphics::CommonStructHelpers::CDepthStencilStateDesc();
		//PSODesc.DepthStencilState.DepthFunc = CF_GreaterEqual;
		PSODesc.BlendState = Graphics::CommonStructHelpers::CBlendDesc();
		PSODesc.RasterizerDesc = Graphics::CommonStructHelpers::CRasterizerDesc();
		PSODesc.SampleMask = UINT_MAX;
		PSODesc.PrimitiveTopologyType = Graphics::PTT_Triangle;
		PSODesc.NumRenderTargets = m_pSwapChain->GetDesc().BufferCount;
		PSODesc.RTVFormats[0] = m_pSwapChain->GetDesc().Format;
		PSODesc.DSVFormat = DCast<Graphics::IPixelBuffer*>(m_pDepthBuffer)->GetFormat();
		PSODesc.SampleDesc = { 1, 0 };
		Graphics::g_pDevice->CreatePipelineState(PSODesc, &m_pScenePassPSO);

		//
		// Init constant buffers
		//
		Graphics::g_pConstantBufferManager->CreateConstantBuffer(TEXT("Scene Constants"), &m_pSceneConstantBuffer, sizeof(SceneConstants));

		Graphics::g_pConstantBufferManager->CreateConstantBuffer(TEXT("Scene Lights"), &m_pLightConstantBuffer, sizeof(SceneLights));


	}

	void WorldRenderer::Render()
	{
		m_GFXTimer.Tick();

		{
			// Process the window's Messages 
			m_pWindow->OnUpdate();

			m_pRenderContext->PreFrame();

			// Render stuff
			Graphics::ICommandContext& CmdContext = Graphics::ICommandContext::Begin(L"Frame");
			{
				// Transition
				Graphics::IColorBuffer* pSwapChainBackBuffer = m_pSwapChain->GetColorBufferForCurrentFrame();
				Graphics::IGPUResource& BackSwapChainBuffer = *DCast<Graphics::IGPUResource*>(pSwapChainBackBuffer);
				CmdContext.TransitionResource(BackSwapChainBuffer, Graphics::RS_RenderTarget);
				Graphics::IGPUResource& DepthBufferResource = *DCast<Graphics::IGPUResource*>(m_pDepthBuffer);
				CmdContext.TransitionResource(DepthBufferResource, Graphics::RS_DepthWrite);

				// TODO: For PostFX
				//const IColorBuffer* RTs[] = { pSceneBuffer };
				//CmdContext.OMSetRenderTargets(1, RTs);
				//CmdContext.ClearColorBuffer(*pSceneBuffer, ScissorRect);

				// Bind
				CmdContext.SetDescriptorHeap(Graphics::RHT_CBV_SRV_UAV, Graphics::g_pTextureHeap);

				// Clear
				CmdContext.ClearColorBuffer(*pSwapChainBackBuffer, m_SceneScissorRect);
				CmdContext.ClearDepth(*m_pDepthBuffer);

				// Set
				const Graphics::IColorBuffer* RTs[] = { pSwapChainBackBuffer };
				CmdContext.OMSetRenderTargets(1, RTs, m_pDepthBuffer);
				CmdContext.RSSetViewPorts(1, &m_SceneViewPort);
				CmdContext.RSSetScissorRects(1, &m_SceneScissorRect);
				CmdContext.SetPipelineState(*m_pScenePassPSO);
				CmdContext.SetGraphicsRootSignature(*m_pCommonRS);


				{
					ieCameraComponent& CurrentCamera = *(m_pWorld->GetCurrentSceneRenderCamera());

					// Set Constant Buffers
					SceneConstants* pData = m_pSceneConstantBuffer->GetBufferPointer<SceneConstants>();
					pData->ViewMat = CurrentCamera.GetViewMatrix().Transpose();
					pData->ProjMat = CurrentCamera.GetProjectionMatrix().Transpose();
					pData->CameraPos = CurrentCamera.GetTransform().GetPosition();
					pData->ViewVector = CurrentCamera.GetTransform().GetLocalForward();
					pData->WorldTime = (float)m_GFXTimer.Seconds();
					CmdContext.SetGraphicsConstantBuffer(SPI_SceneConstants, m_pSceneConstantBuffer);

					SceneLights* pLights = m_pLightConstantBuffer->GetBufferPointer<SceneLights>();
					pLights->PointLights[0].Position = FVector3(-3.f, 0.f, 0.f);
					pLights->PointLights[0].Color = FVector3(1.f, 1.f, 1.f);
					pLights->PointLights[0].Brightness = 7.f;
					CmdContext.SetGraphicsConstantBuffer(SPI_Lights, m_pLightConstantBuffer);
				}

				// Draw world
				RenderStaticMeshGeometry(CmdContext);
				RenderSkinnedMeshGeometry(CmdContext);

				// Present
				CmdContext.TransitionResource(BackSwapChainBuffer, Graphics::RS_Present);
			}
			CmdContext.Finish();

			m_pRenderContext->SubmitFrame();
			m_pRenderContext->Present();
		}

	}
}