#include <Engine_pch.h>

#include "Graphics/Public/WorldRenderer/RendererCommon.h"

#include "Graphics/Public/GraphicsCore.h"


namespace Insight
{
	using namespace Graphics;

	InputElementDesc g_SceneMeshInputElements[6] =
	{
		{ "POSITION",	0, F_R32G32B32_Float,		0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
		{ "NORMAL",		0, F_R32G32B32_Float,		0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
		{ "TANGENT",	0, F_R32G32B32_Float,		0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
		{ "BITANGENT",	0, F_R32G32B32_Float,		0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
		{ "COLOR",		0, F_R32G32B32A32_Float,	0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
		{ "UVS",		0, F_R32G32_Float,			0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
	};
	const UInt32 kNumSceneMeshCommonInputElements = IE_ARRAYSIZE(g_SceneMeshInputElements);

	InputElementDesc g_ScreenSpaceInputElements[2] =
	{
		{ "POSITION",	0, F_R32G32_Float,	0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
		{ "UVS",		0, F_R32G32_Float,	0, IE_APPEND_ALIGNED_ELEMENT,	IC_PerVertexData, 0 },
	};
	const UInt32 kNumScreenSpaceInputElements = IE_ARRAYSIZE(g_ScreenSpaceInputElements);


	constexpr float MinLOD = 0.0f, MaxLOD = 9.0f;
	SamplerDesc g_LinearWrapSamplerDesc
	{ 
		F_Comparison_Min_Mag_Mip_Linear, 
		TAM_Wrap, 
		TAM_Wrap, 
		TAM_Wrap, 
		0,
		1,
		CF_LessEqual,
		{ 0, 0, 0, 0 },
		0.f, // Min LOD
		9.f, // Max LOD
	};
}
