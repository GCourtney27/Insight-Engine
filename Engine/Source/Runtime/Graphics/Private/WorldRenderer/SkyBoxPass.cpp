#include <Engine_pch.h>

#include "Graphics/Public/WorldRenderer/SkyBoxPass.h"

#include "Graphics/Public/WorldRenderer/RendererCommon.h"
#include "Graphics/Public/IPipelineState.h"
#include "Graphics/Public/IRootSignature.h"

#include "Graphics/Public/GeometryGenerator.h"


namespace Insight
{
	SkyboxPass::SkyboxPass()
	{
	}
	
	SkyboxPass::~SkyboxPass()
	{
	}
	
	void SkyboxPass::Initialize(Graphics::IDevice* pDevice, Graphics::EFormat RenderTargetFormat, Graphics::EFormat DepthBufferFormat)
	{
		using namespace Graphics;

		
		m_SkyGeometry = GeometryGenerator::GenerateSphere(10, 20, 20);
		m_SkyDiffuse = Graphics::g_pTextureManager->LoadTexture(L"Content/Textures/Skyboxes/PlainSunset/PlainSunset_Diff.dds", Graphics::DT_Magenta2D, false);

		RootSignatureDesc TEMPDesc = {};
		pDevice->CreateRootSignature(TEMPDesc, &m_pRS);
		m_pRS->Reset(5, 1);
		(*m_pRS).InitStaticSampler(0, g_LinearWrapSamplerDesc, SV_Pixel);
		// Common
		(*m_pRS)[kSceneConstants].InitAsConstantBuffer(kSceneConstants, SV_All);
		(*m_pRS)[kMeshWorld].InitAsConstantBuffer(kMeshWorld, SV_Vertex);
		(*m_pRS)[kMaterial].InitAsConstantBuffer(kMaterial, SV_Pixel);
		(*m_pRS)[kLights].InitAsConstantBuffer(kLights, SV_Pixel);
		// Pipeline
		// Sky Cubemap
		(*m_pRS)[SPRP_Diffuse].InitAsDescriptorTable(1, SV_Pixel);
		(*m_pRS)[SPRP_Diffuse].SetTableRange(0, DRT_ShaderResourceView, 0, 1);
		m_pRS->Finalize(TEXT("Skybox Pass RootSignature"), RSF_AllowInputAssemblerLayout);

		// Create the pipeline state.
		//
		DataBlob VSShader = FileSystem::ReadRawData(L"Shaders/SkyboxPass.vs.cso");
		DataBlob PSShader = FileSystem::ReadRawData(L"Shaders/SkyboxPass.ps.cso");

		InputElementDesc InputElements[1] =
		{
			{ "POSITION",	0, F_R32G32B32_Float,	0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
		};
		const UInt32 kNumInputElements = IE_ARRAYSIZE(InputElements);

		DepthStencilStateDesc DepthStateDesc = {};
		DepthStateDesc.DepthEnable = true;
		DepthStateDesc.DepthWriteMask = DWM_All;
		DepthStateDesc.DepthFunc = CF_LessEqual;

		RasterizerDesc RasterStateDesc = {};
		RasterStateDesc.DepthClipEnabled = true;
		RasterStateDesc.CullMode = CM_Front;
		RasterStateDesc.FillMode = FM_Solid;

		PipelineStateDesc PSODesc = {};
		PSODesc.VertexShader = { VSShader.GetBufferPointer(), VSShader.GetDataSize() };
		PSODesc.PixelShader = { PSShader.GetBufferPointer(), PSShader.GetDataSize() };
		PSODesc.InputLayout.pInputElementDescs = InputElements;
		PSODesc.InputLayout.NumElements = kNumInputElements;
		PSODesc.pRootSignature = m_pRS;
		PSODesc.DepthStencilState = DepthStateDesc;
		PSODesc.BlendState = CommonStructHelpers::CBlendDesc();
		PSODesc.RasterizerDesc = RasterStateDesc;
		PSODesc.SampleMask = UINT_MAX;
		PSODesc.PrimitiveTopologyType = PTT_Triangle;
		PSODesc.NumRenderTargets = 1;
		PSODesc.RTVFormats[0] = RenderTargetFormat;
		PSODesc.SampleDesc = { 1, 0 };
		PSODesc.DSVFormat = DepthBufferFormat;

		pDevice->CreatePipelineState(PSODesc, &m_pPSO);
	}
	
	void SkyboxPass::Set(Graphics::ICommandContext& GfxContext, Graphics::IColorBuffer& RenderTarget, Graphics::IDepthBuffer& DepthBuffer)
	{
		using namespace Graphics;

		GfxContext.BeginDebugMarker(TEXT("Sky Pass"));

		//Transition
		// 
		// Color
		Graphics::IGPUResource& AlbedoGBufferResource = *DCast<Graphics::IGPUResource*>(&RenderTarget);
		GfxContext.TransitionResource(AlbedoGBufferResource, Graphics::RS_RenderTarget);
		// Depth
		IGPUResource& DepthBufferResource = *DCast<IGPUResource*>(&DepthBuffer);
		GfxContext.TransitionResource(DepthBufferResource, RS_DepthWrite);

		// Set
		//
		GfxContext.SetGraphicsRootSignature(*m_pRS);
		GfxContext.SetPipelineState(*m_pPSO);
		
		const Graphics::IColorBuffer* pRTs[] = {
					&RenderTarget,
		};
		GfxContext.OMSetRenderTargets(1, pRTs, &DepthBuffer);


	}

	void SkyboxPass::UnSet(Graphics::ICommandContext& GfxContext, Graphics::IDepthBuffer& DepthBuffer)
	{
		// Render
		//
		GfxContext.SetTexture(SPRP_Diffuse, m_SkyDiffuse);

		if (m_SkyGeometry.IsValid())
		{
			GfxContext.SetPrimitiveTopologyType(Graphics::PT_TiangleList);
			GfxContext.BindVertexBuffer(0, m_SkyGeometry->GetVertexBuffer());
			GfxContext.BindIndexBuffer(m_SkyGeometry->GetIndexBuffer());
			GfxContext.DrawIndexedInstanced(m_SkyGeometry->GetNumIndices(), 1, 0, 0, 0);
		}
		else
			IE_LOG(Warning, TEXT("Trying to render a skybox with invalid geometry!"));

		// Transition
		//
		Graphics::IGPUResource& DepthBufferResource = *DCast<Graphics::IGPUResource*>(&DepthBuffer);
		GfxContext.TransitionResource(DepthBufferResource, Graphics::RS_DepthRead);

		GfxContext.EndDebugMarker();
	}
}