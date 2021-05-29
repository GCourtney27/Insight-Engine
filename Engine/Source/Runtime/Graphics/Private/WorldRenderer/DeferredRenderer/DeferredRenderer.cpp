#include <Engine_pch.h>

#include "Runtime/Graphics/Public/WorldRenderer/DeferredRenderer/DeferredRenderer.h"

#include "Runtime/Graphics/Public/IDevice.h"

#include "Runtime/Graphics/Public/WorldRenderer/Common.h"
#include "Runtime/Graphics/Public/IPipelineState.h"
#include "Runtime/Graphics/Public/IRootSignature.h"

namespace Insight
{
	//
	// Deferred Renderer Implementation
	//

	void DeferredShadingPipeline::Initialize(ieWorld* pWorld, FVector2 RenderResolution, Graphics::IDevice* pDevice, Graphics::EFormat SwapchainFormatTEMP)
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

		constexpr float MinLOD = 0.0f, MaxLOD = 9.0f;
		SamplerDesc LinearWrapSamplerDesc;
		LinearWrapSamplerDesc.Filter = F_Comparison_Min_Mag_Mip_Linear;
		LinearWrapSamplerDesc.AddressU = TAM_Wrap;
		LinearWrapSamplerDesc.AddressV = TAM_Wrap;
		LinearWrapSamplerDesc.AddressW = TAM_Wrap;
		LinearWrapSamplerDesc.MipLODBias = 0;
		LinearWrapSamplerDesc.MaxAnisotropy = 1;
		LinearWrapSamplerDesc.ComparisonFunc = CF_LessEqual;
		LinearWrapSamplerDesc.MinLOD = MinLOD;
		LinearWrapSamplerDesc.MaxLOD = MaxLOD;


		RootSignatureDesc TEMPDesc = {};
		pDevice->CreateRootSignature(TEMPDesc, &m_pRS);
		m_pRS->Reset(6, 1);
		(*m_pRS).InitStaticSampler(0, LinearWrapSamplerDesc, SV_Pixel);
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
		InputElementDesc InputElements[] =
		{
			{ "POSITION",	0, F_R32G32B32_Float,		0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
			{ "NORMAL",		0, F_R32G32B32_Float,		0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
			{ "TANGENT",	0, F_R32G32B32_Float,		0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
			{ "BITANGENT",	0, F_R32G32B32_Float,		0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
			{ "COLOR",		0, F_R32G32B32A32_Float,	0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
			{ "UVS",		0, F_R32G32_Float,			0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
		};

		size_t VSShaderSize = 0;
		ByteArray VSShader = FileSystem::ReadRawData(L"Shaders/GenericGeometryPass.vs.cso", VSShaderSize);

		size_t PSShaderSize = 0;
		ByteArray PSShader = FileSystem::ReadRawData(L"Shaders/GenericGeometryPass.ps.cso", PSShaderSize);


		PipelineStateDesc PSODesc = {};
		PSODesc.VertexShader = { VSShader.get()->data(), VSShaderSize };
		PSODesc.PixelShader = { PSShader.get()->data(), PSShaderSize };
		PSODesc.InputLayout.NumElements = _countof(InputElements);
		PSODesc.InputLayout.pInputElementDescs = InputElements;
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
		IGPUResource& DepthBufferResource = *DCast<IGPUResource*>(m_pDepthBuffer);
		GfxContext.TransitionResource(DepthBufferResource, RS_DepthWrite);

		// Clear
		//
		GfxContext.ClearColorBuffer(GetGBufferColorBuffer(GB_Albedo), Viewrect);
		GfxContext.ClearColorBuffer(GetGBufferColorBuffer(GB_Normal), Viewrect);
		GfxContext.ClearColorBuffer(GetGBufferColorBuffer(GB_Position), Viewrect);
		GfxContext.ClearDepth(*m_pDepthBuffer);


		// Set
		//
		const IColorBuffer* RTs[] =
		{
			m_RenderTargets[0], m_RenderTargets[1], m_RenderTargets[2]
		};
		GfxContext.OMSetRenderTargets(
			GB_NumBuffers,
			RTs,
			m_pDepthBuffer);
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
	}


	//
	// Light Pass Implementation
	//


	void DeferredShadingPipeline::LightPass::Initialize(Graphics::IDevice* pDevice, const FVector2& RenderResolution, Graphics::EFormat SwapchainFormatTEMP)
	{
		using namespace Graphics;

		{
			constexpr float MinLOD = 0.0f, MaxLOD = 9.0f;
			SamplerDesc LinearWrapSamplerDesc;
			LinearWrapSamplerDesc.Filter = F_Comparison_Min_Mag_Mip_Linear;
			LinearWrapSamplerDesc.AddressU = TAM_Wrap;
			LinearWrapSamplerDesc.AddressV = TAM_Wrap;
			LinearWrapSamplerDesc.AddressW = TAM_Wrap;
			LinearWrapSamplerDesc.MipLODBias = 0;
			LinearWrapSamplerDesc.MaxAnisotropy = 1;
			LinearWrapSamplerDesc.ComparisonFunc = CF_LessEqual;
			LinearWrapSamplerDesc.MinLOD = MinLOD;
			LinearWrapSamplerDesc.MaxLOD = MaxLOD;


			RootSignatureDesc TEMPDesc = {};
			pDevice->CreateRootSignature(TEMPDesc, &m_pRS);
			m_pRS->Reset(8, 1);
			(*m_pRS).InitStaticSampler(0, LinearWrapSamplerDesc, SV_Pixel);
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
		}


		// Create the pipeline state.
		//
		InputElementDesc InputElements[] =
		{
			{ "POSITION",	0, F_R32G32_Float,	0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
			{ "UVS",		0, F_R32G32_Float,	0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
		};


		size_t VSShaderSize = 0;
		ByteArray VSShader = FileSystem::ReadRawData(L"Shaders/GenericLightPass.vs.cso", VSShaderSize);

		size_t PSShaderSize = 0;
		ByteArray PSShader = FileSystem::ReadRawData(L"Shaders/GenericLightPass.ps.cso", PSShaderSize);


		PipelineStateDesc PSODesc = {};
		PSODesc.VertexShader = { VSShader.get()->data(), VSShaderSize };
		PSODesc.PixelShader = { PSShader.get()->data(), PSShaderSize };
		PSODesc.InputLayout.NumElements = _countof(InputElements);
		PSODesc.InputLayout.pInputElementDescs = InputElements;
		PSODesc.pRootSignature = m_pRS;
		PSODesc.DepthStencilState.DepthEnable = false;
		PSODesc.BlendState = CommonStructHelpers::CBlendDesc();
		PSODesc.RasterizerDesc = CommonStructHelpers::CRasterizerDesc();
		PSODesc.SampleMask = UINT_MAX;
		PSODesc.PrimitiveTopologyType = PTT_Triangle;
		PSODesc.NumRenderTargets = 1; // The Swapchain 
		PSODesc.RTVFormats[0] = SwapchainFormatTEMP;
		PSODesc.SampleDesc = { 1, 0 };
		pDevice->CreatePipelineState(PSODesc, &m_pPSO);
	}

	void DeferredShadingPipeline::LightPass::UnInitialize()
	{
		SAFE_DELETE_PTR(m_pRS);
		SAFE_DELETE_PTR(m_pPSO);
		// TODO: SAFE_DELETE_PTR(m_LightPassResult);
	}

	void DeferredShadingPipeline::LightPass::Set(Graphics::ICommandContext& GfxContext, Graphics::Rect Viewrect, const DeferredShadingPipeline::GeometryPass& GeometryPass)
	{
		GfxContext.SetPipelineState(*m_pPSO);
		GfxContext.SetGraphicsRootSignature(*m_pRS);

		GfxContext.SetDepthBufferAsTexture(LRP_GBufferTextureSceneDepth, GeometryPass.GetSceneDepthBuffer());
		GfxContext.SetColorBufferAsTexture(LRP_GBufferTextureAlbedo, 0, &GeometryPass.GetGBufferColorBuffer(GB_Albedo));
		GfxContext.SetColorBufferAsTexture(LRP_GBufferTextureNormal, 0, &GeometryPass.GetGBufferColorBuffer(GB_Normal));
		GfxContext.SetColorBufferAsTexture(LRP_GBufferTexturePosition, 0, &GeometryPass.GetGBufferColorBuffer(GB_Position));
	}

	void DeferredShadingPipeline::LightPass::UnSet(Graphics::ICommandContext& GfxContext)
	{
	}
}
