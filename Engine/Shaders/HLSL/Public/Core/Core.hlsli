#include "../../../../Source/Runtime/EngineDefines.h"
#include "LightsFwd.hlsli"

//
// Core Types and Macros for Rendering 
//

// Constant Buffers
cbuffer SceneConstants      : register(kSceneConstantsReg)
{
    float4x4 ViewMat;
    float4x4 ProjMat;
    float4x4 InverseViewMat;
    float4x4 InverseProjMat;
    float3 CameraPos;
    float3 ViewVector;
    float WorldTime;
    float CameraNearZ;
    float CameraFarZ;
}
cbuffer MeshWorld           : register(kMeshWorldReg)
{
    float4x4 WorldMat;
}
cbuffer MaterialConstants   : register(kMaterialReg)
{
    float4 MatColor;
};
cbuffer SceneLights         : register(kLightsReg)
{
    uint NumPointLights;
    uint NumDirectionalLights;
	/* float Unused0[2]; */

    PointLight PointLights[IE_MAX_POINT_LIGHTS];
	DirectionalLight DirectionalLights[IE_MAX_DIRECTIONAL_LIGHTS];
}