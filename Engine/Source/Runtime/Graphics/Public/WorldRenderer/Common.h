// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include <Runtime/Core.h>
#include "Runtime/Math/Public/Matricies.h"
#include "Runtime/Math/Public/Vectors.h"

namespace Insight
{
	enum EShaderParamIndices
	{
		SPI_SceneConstants = 0,
		SPI_MeshWorld = 1,
		SPI_MaterialParams = 2,
		SPI_Lights = 3,
		SPI_Texture_Albedo = 4,
		SPI_Texture_Normal = 5,
	};

	ALIGN(16) struct SceneConstants
	{
		FMatrix ViewMat;
		FMatrix ProjMat;
		FVector3 CameraPos;
		FVector3 ViewVector;
		float WorldTime;
	};
	ALIGN(16) struct MeshWorld
	{
		FMatrix WorldMat;
	};
	ALIGN(16) struct MaterialConstants
	{
		FVector4 Color;
	};
#define IE_MAX_POINT_LIGHTS 4
#define IE_MAX_SPOT_LIGHTS 4
#define IE_MAX_DIRECTIONAL_LIGHTS 4
	ALIGN(16) struct PointLight
	{
		FVector3 Position;
		FVector3 Color;
		float pad;
		float Brightness;
	};
	struct SceneLights
	{
		PointLight PointLights[IE_MAX_POINT_LIGHTS];
	};

}
