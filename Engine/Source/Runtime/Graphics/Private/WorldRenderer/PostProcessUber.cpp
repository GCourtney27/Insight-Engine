#include <Engine_pch.h>

#include "Runtime/Graphics/Public/WorldRenderer/PostProcessUber.h"

#include "Runtime/Graphics/Public/WorldRenderer/RendererCommon.h"
#include "Runtime/Graphics/Public/IPipelineState.h"
#include "Runtime/Graphics/Public/IRootSignature.h"


namespace Insight
{
	void PostProcessUber::Initialize(FVector2 RenderResolution, Graphics::IDevice* pDevice)
	{
		using namespace Graphics;

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

		// Create the render targets.
		//
		pDevice->CreateColorBuffer(TEXT("[Light Pass] Result"), (UInt32)RenderResolution.x, (UInt32)RenderResolution.y, 1, F_R32G32B32A32_Float, &m_pPassResult);

	}
}
