#include <Engine_pch.h>

#include "Graphics/Public/WorldRenderer/DeferredRenderer/DeferredRenderer.h"

#include "Graphics/Public/IDevice.h"

#include "Graphics/Public/WorldRenderer/RendererCommon.h"
#include "Graphics/Public/IPipelineState.h"
#include "Graphics/Public/IRootSignature.h"


namespace Insight
{
	//
	// Deferred Renderer Implementation
	//

	void DeferredShadingPipeline::Initialize(FVector2 RenderResolution, Graphics::IDevice* pDevice, Graphics::EFormat SwapchainFormatTEMP)
	{
		m_GeometryPass.Initialize(pDevice, RenderResolution);
		m_LightPass.Initialize(pDevice, RenderResolution, SwapchainFormatTEMP);
	}

	void DeferredShadingPipeline::UnInitialize()
	{
		m_GeometryPass.UnInitialize();
		m_LightPass.UnInitialize();
	}

	//
	// Geometry Pass Implementation
	//

	void DeferredShadingPipeline::GeometryPass::Initialize(
		Graphics::IDevice* pDevice,
		const FVector2& RenderResolution)
	{
		using namespace Graphics;

		RootSignatureDesc TEMPDesc = {};
		pDevice->CreateRootSignature(TEMPDesc, &m_pRS);
		m_pRS->Reset(6, 1);
		(*m_pRS).InitStaticSampler(0, g_LinearWrapSamplerDesc, SV_Pixel);
		// Common
		(*m_pRS)[kSceneConstants].InitAsConstantBuffer(kSceneConstants, SV_All);
		(*m_pRS)[kMeshWorld].InitAsConstantBuffer(kMeshWorld, SV_Vertex);
		(*m_pRS)[kMaterial].InitAsConstantBuffer(kMaterial, SV_Pixel);
		(*m_pRS)[kLights].InitAsConstantBuffer(kLights, SV_Pixel);
		// Pipeline
		// Albedo
		(*m_pRS)[GRP_MaterialTextureAlbedo].InitAsDescriptorTable(1, SV_Pixel);
		(*m_pRS)[GRP_MaterialTextureAlbedo].SetTableRange(0, DRT_ShaderResourceView, 0, 1);
		// Normal
		(*m_pRS)[GRP_MaterialTextureNormal].InitAsDescriptorTable(1, SV_Pixel);
		(*m_pRS)[GRP_MaterialTextureNormal].SetTableRange(0, DRT_ShaderResourceView, 1, 1);
		(*m_pRS).Finalize(L"[Deferred Renderer][Geometry Pass] RootSig", RSF_AllowInputAssemblerLayout);


		// Create the render targets.
		//
		const UInt32 NumMips = 1u;
		pDevice->CreateColorBuffer(TEXT("[Geometry Pass][G-Buffer] Albedo Buffer"), (UInt32)RenderResolution.x, (UInt32)RenderResolution.y, NumMips, F_R8G8B8A8_UNorm, &m_RenderTargets[GB_Albedo]);
		pDevice->CreateColorBuffer(TEXT("[Geometry Pass][G-Buffer] Normal Buffer"), (UInt32)RenderResolution.x, (UInt32)RenderResolution.y, NumMips, F_R8G8B8A8_UNorm, &m_RenderTargets[GB_Normal]);
		pDevice->CreateColorBuffer(TEXT("[Geometry Pass][G-Buffer] Position Buffer"), (UInt32)RenderResolution.x, (UInt32)RenderResolution.y, NumMips, F_R32G32B32A32_Float, &m_RenderTargets[GB_Position]);


		// Create the depth buffer.
		//
		pDevice->CreateDepthBuffer(TEXT("[Geometry Pass] Scene Depth Buffer"), (UInt32)RenderResolution.x, (UInt32)RenderResolution.y, F_D32_Float, &m_pDepthBuffer);


		// Create the pipeline state.
		//
		DataBlob VSShader = FileSystem::ReadRawData(L"Shaders/GenericGeometryPass.vs.cso");
		DataBlob PSShader = FileSystem::ReadRawData(L"Shaders/GenericGeometryPass.ps.cso");


		PipelineStateDesc PSODesc = {};
		PSODesc.VertexShader = { VSShader.GetBufferPointer(), VSShader.GetDataSize() };
		PSODesc.PixelShader = { PSShader.GetBufferPointer(), PSShader.GetDataSize() };
		PSODesc.InputLayout.pInputElementDescs = g_SceneMeshInputElements;
		PSODesc.InputLayout.NumElements = kNumSceneMeshCommonInputElements;
		PSODesc.pRootSignature = m_pRS;
		PSODesc.DepthStencilState = CommonStructHelpers::CDepthStencilStateDesc();
		//PSODesc.DepthStencilState.DepthFunc = CF_GreaterEqual;
		PSODesc.BlendState = CommonStructHelpers::CBlendDesc();
		PSODesc.RasterizerDesc = CommonStructHelpers::CRasterizerDesc();
		PSODesc.SampleMask = UINT_MAX;
		PSODesc.PrimitiveTopologyType = PTT_Triangle;
		PSODesc.NumRenderTargets = GB_NumBuffers;
		for (UInt32 i = 0; i < GB_NumBuffers; ++i)
		{
			PSODesc.RTVFormats[i] = DCast<IPixelBuffer*>(m_RenderTargets[i])->GetFormat();
		}
		PSODesc.DSVFormat = DCast<IPixelBuffer*>(m_pDepthBuffer)->GetFormat();
		PSODesc.SampleDesc = { 1, 0 };
		pDevice->CreatePipelineState(PSODesc, &m_pPSO);


		m_pDepthBufferGPUResource = DCast<Graphics::IGPUResource*>(m_pDepthBuffer);;
	}

	void DeferredShadingPipeline::GeometryPass::UnInitialize()
	{
		SAFE_DELETE_PTR(m_pDepthBuffer);
		SAFE_DELETE_PTR(m_pPSO);
		SAFE_DELETE_PTR(m_pRS);
		SAFE_DELETE_PTR(m_RenderTargets[0]);
		SAFE_DELETE_PTR(m_RenderTargets[1]);
	}

	void DeferredShadingPipeline::GeometryPass::Set(Graphics::ICommandContext& GfxContext, Graphics::Rect Viewrect)
	{
		using namespace Graphics;
		GfxContext.BeginDebugMarker(TEXT("Geometry Pass"));

		// Transition
		// 
		// Color
		IGPUResource& AlbedoGBufferResource = *DCast<IGPUResource*>(&GetGBufferColorBuffer(GB_Albedo));
		GfxContext.TransitionResource(AlbedoGBufferResource, RS_RenderTarget);
		IGPUResource& NormalGBufferResource = *DCast<IGPUResource*>(&GetGBufferColorBuffer(GB_Normal));
		GfxContext.TransitionResource(NormalGBufferResource, RS_RenderTarget);
		IGPUResource& PositionGBufferResource = *DCast<IGPUResource*>(&GetGBufferColorBuffer(GB_Position));
		GfxContext.TransitionResource(PositionGBufferResource, RS_RenderTarget);
		// Depth
		IGPUResource& DepthBufferResource = *DCast<IGPUResource*>(GetSceneDepthBuffer());
		GfxContext.TransitionResource(DepthBufferResource, RS_DepthWrite);

		// Clear
		//
		GfxContext.ClearColorBuffer(GetGBufferColorBuffer(GB_Albedo), Viewrect);
		GfxContext.ClearColorBuffer(GetGBufferColorBuffer(GB_Normal), Viewrect);
		GfxContext.ClearColorBuffer(GetGBufferColorBuffer(GB_Position), Viewrect);
		GfxContext.ClearDepth(*GetSceneDepthBuffer());


		// Set
		//
		const IColorBuffer* RTs[] =
		{
			m_RenderTargets[0], m_RenderTargets[1], m_RenderTargets[2]
		};
		GfxContext.OMSetRenderTargets(
			GB_NumBuffers,
			RTs,
			GetSceneDepthBuffer());
		GfxContext.SetPipelineState(*m_pPSO);
		GfxContext.SetGraphicsRootSignature(*m_pRS);

	}

	void DeferredShadingPipeline::GeometryPass::UnSet(Graphics::ICommandContext& GfxContext)
	{
		Graphics::IGPUResource& AlbedoGBufferResource = *DCast<Graphics::IGPUResource*>(&GetGBufferColorBuffer(GB_Albedo));
		GfxContext.TransitionResource(AlbedoGBufferResource, Graphics::RS_PixelShaderResource);

		Graphics::IGPUResource& NormalGBufferResource = *DCast<Graphics::IGPUResource*>(&GetGBufferColorBuffer(GB_Normal));
		GfxContext.TransitionResource(NormalGBufferResource, Graphics::RS_PixelShaderResource);

		Graphics::IGPUResource& PositionGBufferResource = *DCast<Graphics::IGPUResource*>(&GetGBufferColorBuffer(GB_Position));
		GfxContext.TransitionResource(PositionGBufferResource, Graphics::RS_PixelShaderResource);

		Graphics::IGPUResource& DepthBuffferResource = *DCast<Graphics::IGPUResource*>(GetSceneDepthBuffer());
		GfxContext.TransitionResource(DepthBuffferResource, Graphics::RS_DepthRead);

		GfxContext.EndDebugMarker();
	}


	//
	// Light Pass Implementation
	//


	void DeferredShadingPipeline::LightPass::Initialize(Graphics::IDevice* pDevice, const FVector2& RenderResolution, Graphics::EFormat SwapchainFormatTEMP)
	{
		using namespace Graphics;

		// Create the render targets.
		//
		pDevice->CreateColorBuffer(TEXT("[Light Pass] Result"), (UInt32)RenderResolution.x, (UInt32)RenderResolution.y, 1, F_R32G32B32A32_Float, &m_pPassResult);

		RootSignatureDesc TEMPDesc = {};
		pDevice->CreateRootSignature(TEMPDesc, &m_pRS);
		m_pRS->Reset(8, 1);
		(*m_pRS).InitStaticSampler(0, g_LinearWrapSamplerDesc, SV_Pixel);
		// Common
		(*m_pRS)[kSceneConstants].InitAsConstantBuffer(kSceneConstants, SV_All);
		(*m_pRS)[kMeshWorld].InitAsConstantBuffer(kMeshWorld, SV_Vertex);
		(*m_pRS)[kMaterial].InitAsConstantBuffer(kMaterial, SV_Pixel);
		(*m_pRS)[kLights].InitAsConstantBuffer(kLights, SV_Pixel);
		// Pipeline
		// Scene Depth
		(*m_pRS)[LRP_GBufferTextureSceneDepth].InitAsDescriptorTable(1, SV_Pixel);
		(*m_pRS)[LRP_GBufferTextureSceneDepth].SetTableRange(0, DRT_ShaderResourceView, 0, 1);
		// Albedo
		(*m_pRS)[LRP_GBufferTextureAlbedo].InitAsDescriptorTable(1, SV_Pixel);
		(*m_pRS)[LRP_GBufferTextureAlbedo].SetTableRange(0, DRT_ShaderResourceView, 1, 1);
		// Normal
		(*m_pRS)[LRP_GBufferTextureNormal].InitAsDescriptorTable(1, SV_Pixel);
		(*m_pRS)[LRP_GBufferTextureNormal].SetTableRange(0, DRT_ShaderResourceView, 2, 1);
		// Position
		(*m_pRS)[LRP_GBufferTexturePosition].InitAsDescriptorTable(1, SV_Pixel);
		(*m_pRS)[LRP_GBufferTexturePosition].SetTableRange(0, DRT_ShaderResourceView, 3, 1);
		(*m_pRS).Finalize(L"[Deferred Renderer][Light Pass] RootSig", RSF_AllowInputAssemblerLayout);


		// Create the pipeline state.
		//
		DataBlob VSShader = FileSystem::ReadRawData(L"Shaders/GenericLightPass.vs.cso");
		DataBlob PSShader = FileSystem::ReadRawData(L"Shaders/GenericLightPass.ps.cso");

		PipelineStateDesc PSODesc = {};
		PSODesc.VertexShader = { VSShader.GetBufferPointer(), VSShader.GetDataSize() };
		PSODesc.PixelShader = { PSShader.GetBufferPointer(), PSShader.GetDataSize() };
		PSODesc.InputLayout.NumElements = kNumScreenSpaceInputElements;
		PSODesc.InputLayout.pInputElementDescs = g_ScreenSpaceInputElements;
		PSODesc.pRootSignature = m_pRS;
		PSODesc.DepthStencilState.DepthEnable = false;
		PSODesc.BlendState = CommonStructHelpers::CBlendDesc();
		PSODesc.RasterizerDesc = CommonStructHelpers::CRasterizerDesc();
		PSODesc.SampleMask = UINT_MAX;
		PSODesc.PrimitiveTopologyType = PTT_Triangle;
		PSODesc.NumRenderTargets = 1;
		PSODesc.RTVFormats[0] = SwapchainFormatTEMP;// GetPassResultFormat();
		PSODesc.SampleDesc = { 1, 0 };
		pDevice->CreatePipelineState(PSODesc, &m_pPSO);
	}

	void DeferredShadingPipeline::LightPass::UnInitialize()
	{
		SAFE_DELETE_PTR(m_pRS);
		SAFE_DELETE_PTR(m_pPSO);
		SAFE_DELETE_PTR(m_pPassResult);
	}

	void DeferredShadingPipeline::LightPass::Set(Graphics::ICommandContext& GfxContext, Graphics::Rect Viewrect, const DeferredShadingPipeline::GeometryPass& GeometryPass)
	{
		GfxContext.BeginDebugMarker(TEXT("Light Pass"));

		GfxContext.SetPipelineState(*m_pPSO);
		GfxContext.SetGraphicsRootSignature(*m_pRS);

		GfxContext.SetDepthBufferAsTexture(LRP_GBufferTextureSceneDepth, GeometryPass.GetSceneDepthBuffer());
		GfxContext.SetColorBufferAsTexture(LRP_GBufferTextureAlbedo, 0, &GeometryPass.GetGBufferColorBuffer(GB_Albedo));
		GfxContext.SetColorBufferAsTexture(LRP_GBufferTextureNormal, 0, &GeometryPass.GetGBufferColorBuffer(GB_Normal));
		GfxContext.SetColorBufferAsTexture(LRP_GBufferTexturePosition, 0, &GeometryPass.GetGBufferColorBuffer(GB_Position));
	}

	void DeferredShadingPipeline::LightPass::UnSet(Graphics::ICommandContext& GfxContext)
	{
		GfxContext.EndDebugMarker();
	}
}
