// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include "EngineDefines.h"

#include "Math/Public/Matricies.h"
#include "Math/Public/Vectors.h"
#include "Graphics/Public/CommonStructs.h"
#include "Core/Public/Handle.h"

namespace Insight
{

	// -----------
	// Handles
	// -----------
	//
	typedef Handle SpotLightDataHandle;
	typedef Handle PointLightDataHandle;
	typedef Handle DirectionalLightDataHandle;

#	define IE_INVALID_SPOT_LIGHT_HANDLE			( SpotLightDataHandle		(-1) )
#	define IE_INVALID_POINT_LIGHT_HANDLE		( PointLightDataHandle		(-1) )
#	define IE_INVALID_DIRECTIONAL_LIGHT_HANDLE	( DirectionalLightDataHandle(-1) )

	const float kDefaultBrightness = 2000.f;


	/*
		Common root parameters that each root signature MUST use.
		If a pass requeres additional root parameters (textures, buffers, etc.)
		their first element must be equal to `kNumCommonRootParams`. The order after
		that does not matter and is up to the user.
	*/
	enum ECommonRootParamsBuffers
	{
		kSceneConstants = 0,
		kMeshWorld = 1,
		kMaterial = 2,
		kLights = 3,

		kNumCommonRootParams = kLights + 1,
	};


	//
	// Geometry Pass
	//
	enum EGeometryPassRootParams
	{
		GRP_MaterialTextureAlbedo = kLights + 1,
		GRP_MaterialTextureNormal = GRP_MaterialTextureAlbedo + 1,

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
		LRP_GBufferTextureSceneDepth = kLights + 1,
		LRP_GBufferTextureAlbedo = LRP_GBufferTextureSceneDepth + 1,
		LRP_GBufferTextureNormal = LRP_GBufferTextureAlbedo + 1,
		LRP_GBufferTexturePosition = LRP_GBufferTextureNormal + 1,
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


	//
	// Sky Pass
	//
	enum ESkyPassRootParams
	{
		SPRP_Diffuse = kLights + 1,
	};
	enum ESkyPassDEscriptorRanges
	{
		SPDR_Diffuse = 0,
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

	IE_ALIGN(16) struct PointLightData
	{
		// The position of the light in world space.
		FVector4				Position;
		
		// The color of the light. Fourth component is unused.
		FVector4				Color;
		
		// The brightness the light will illuminate.
		float					Brightness;
		
		// The unique identifier of the light.
		PointLightDataHandle	Id;
	};
	IE_ALIGN(16) struct DirectionalLightData
	{
		// The direction of the light.
		FVector4						Direction;
		
		// The color of the light.
		FVector4						Color;

		// The brightness the light will illuminate.
		float							Brightness;

		// The unique identifier of the light.
		DirectionalLightDataHandle		Id;
	};
	IE_ALIGN(16) struct SpotLightData
	{
		float					TEMP;
		SpotLightDataHandle		Id;
	};
	struct SceneLights
	{
		UInt32					NumPointLights;
		UInt32					NumDirectionalLights;
		float					Unused0[2];

		PointLightData			PointLights[IE_MAX_POINT_LIGHTS];
		DirectionalLightData	DirectionalLights[IE_MAX_DIRECTIONAL_LIGHTS];
		//TODO: SpotLightData			SpotLights[IE_MAX_SPOT_LIGHTS];
	};

	// Input layouts.
	extern Graphics::InputElementDesc g_SceneMeshInputElements[];
	extern const UInt32 kNumSceneMeshCommonInputElements;

	extern Graphics::InputElementDesc g_ScreenSpaceInputElements[];
	extern const UInt32 kNumScreenSpaceInputElements;

	// Sampler Descriptions.
	extern Graphics::SamplerDesc g_LinearWrapSamplerDesc;

#undef MAKE_LIGHT_HANDLE
}