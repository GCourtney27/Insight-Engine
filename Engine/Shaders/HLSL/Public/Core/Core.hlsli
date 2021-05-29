//#include "EngineDefines.h"
#include "LightsFwd.hlsli"

//
// Core Types and Macros for Rendering 
//

#define kSceneConstantsReg  b0
#define kMeshWorldReg       b1
#define kMaterialReg        b2
#define kLightsReg          b3

#if IE_PLATFORM_BUILD_WINDOWS
#   define ConstantBuffer(Slot) register(Slot) 
#endif

// Constant Buffers
cbuffer SceneConstants : register(kSceneConstantsReg)
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
};
cbuffer MeshWorld : register(kMeshWorldReg)
{
    float4x4 WorldMat;
}
cbuffer MaterialConstants : register(kMaterialReg)
{
    float4 MatColor;
};
cbuffer SceneLights : register(kLightsReg)
{
    PointLight PointLights[4];
}