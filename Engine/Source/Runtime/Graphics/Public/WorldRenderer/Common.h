// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include <Runtime/Core.h>
#include "Runtime/Math/Public/Matricies.h"
#include "Runtime/Math/Public/Vectors.h"

namespace Insight
{
	/*
		Common root parameters that each root signature MUST use.
		If a pass requeres additional root parameters (textures, buffers, etc.)
		their first element must be equal to `kNumCommonRootParams`. The order after 
		that does not matter and is up to the user.
	*/
	enum ECommonRootParamsBuffers
	{
		kSceneConstants = 0,
		kMeshWorld		= 1,
		kMaterial		= 2,
		kLights			= 3,

		kNumCommonRootParams = kLights + 1, 
	};


	//
	// Geometry Pass
	//
	enum EGeometryPassRootParams
	{
		GRP_MaterialTextureAlbedo	= kLights + 1,
		GRP_MaterialTextureNormal	= GRP_MaterialTextureAlbedo + 1,

		GRP_NumParams = 2,
	};
	enum EGeometryPassDescriptorRanges
	{
		GDR_MaterialTextureAlbedo = 0,
		GDR_MaterialTextureNormal = 1,
		//GDR_Texture_Metallic = 2,
		//GDR_Texture_Roughness = 3,

		GDR_RangeCount = 2,
	};


	//
	// Light Pass
	//
	enum ELightPassRootParams
	{
		LRP_GBufferTextureSceneDepth	= kLights + 1,
		LRP_GBufferTextureAlbedo		= LRP_GBufferTextureSceneDepth + 1,
		LRP_GBufferTextureNormal		= LRP_GBufferTextureAlbedo + 1,
		LRP_GBufferTexturePosition		= LRP_GBufferTextureNormal + 1,
		//LRP_GBufferTextureMetallic	= 2,
		//LRP_GBufferTextureRoughness	= 3,

		LRP_NumParams = 3,
	};
	enum ELightPassDescriptorRanges
	{
		LDR_GBufferTextureAlbedo = 0,
		LDR_GBufferTextureNormal = 1,
		//GDR_Texture_Metallic = 2,
		//GDR_Texture_Roughness = 3,

		LDR_RangeCount = 2,
	};



	IE_ALIGN(16) struct SceneConstants
	{
		FMatrix ViewMat;
		FMatrix ProjMat;
		FMatrix InverseViewMat;
		FMatrix InverseProjMat;
		FVector3 CameraPos;
		float WorldTime;
		float CameraNearZ;
		float CameraFarZ;
	};
	IE_ALIGN(16) struct MeshWorld
	{
		FMatrix WorldMat;
	};
	IE_ALIGN(16) struct MaterialConstants
	{
		FVector4 Color;
	};

#define IE_MAX_POINT_LIGHTS 4
#define IE_MAX_SPOT_LIGHTS 4
#define IE_MAX_DIRECTIONAL_LIGHTS 4
	IE_ALIGN(16) struct PointLight
	{
		FVector4 Position;
		FVector4 Color;
		float Brightness;
		float Unused0[3];
	};
	IE_ALIGN(16) struct DirectionalLight
	{
		FVector3 Direction;
		float Brightness;
	};
	struct SceneLights
	{
		PointLight PointLights[IE_MAX_POINT_LIGHTS];
		//DirectionalLight DirectionalLights[IE_MAX_DIRECTIONAL_LIGHTS];
	};

	/*
		Populates the first `kNumCommonRootParams` elements in the provided root parameter array with common 
		RootParameter definitions.
		This MUST be called when creating new RootParameter arrays for them to conform to the rendering pipeline.
		@param pRootParams - A array of root parameters to initialize. The provided array size must be greater 
		than or equal to `kNumCommonRootParams`
	*/
	//void InitCommonRootSignatureFields(Graphics::RootParameter* pRootParams);
}

/*
{
		using namespace Graphics;

		ZeroMem(pRootParams, kNumCommonRootParams * sizeof(RootParameter));
		// Scene Constants
		pRootParams[kSceneConstants].ShaderVisibility = SV_All;
		pRootParams[kSceneConstants].ParameterType = RPT_ConstantBufferView;
		pRootParams[kSceneConstants].Descriptor.ShaderRegister = kSceneConstants;
		pRootParams[kSceneConstants].Descriptor.RegisterSpace = 0;
		// Mesh World
		pRootParams[kMeshWorld].ShaderVisibility = SV_Vertex;
		pRootParams[kMeshWorld].ParameterType = RPT_ConstantBufferView;
		pRootParams[kMeshWorld].Descriptor.ShaderRegister = kMeshWorld;
		pRootParams[kMeshWorld].Descriptor.RegisterSpace = 0;
		// Material
		pRootParams[kMaterial].ShaderVisibility = SV_Pixel;
		pRootParams[kMaterial].ParameterType = RPT_ConstantBufferView;
		pRootParams[kMaterial].Descriptor.ShaderRegister = kMaterial;
		pRootParams[kMaterial].Descriptor.RegisterSpace = 0;
		// Lights
		pRootParams[kLights].ShaderVisibility = SV_All;
		pRootParams[kLights].ParameterType = RPT_ConstantBufferView;
		pRootParams[kLights].Descriptor.ShaderRegister = kLights;
		pRootParams[kLights].Descriptor.RegisterSpace = 0;
	}
*/
