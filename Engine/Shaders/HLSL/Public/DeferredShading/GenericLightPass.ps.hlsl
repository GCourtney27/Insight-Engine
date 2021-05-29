#include "../Core/Core.hlsli"
#include "DeferredShadingCommon.hlsli"


// Samplers
//
SamplerState g_LinearWrapSampler : register(s0);

// GBuffer Inputs
//
Texture2D g_SceneDepth : register(t0);
Texture2D g_AlbedoGBuffer : register(t1);
Texture2D g_NormalGBuffer : register(t2);
Texture2D g_PositionGBuffer : register(t3);

// Forward Function Declarations
//
float3 WorldPosFromDepth(float _Depth, float2 _TexCoords);
float LinearizeDepth(float depth);

//
// Entry Point
//
LP_PSOutput main(LP_PSInput Input)
{
    LP_PSOutput Output;

    float3 AlbedoSample = g_AlbedoGBuffer.Sample(g_LinearWrapSampler, Input.UVs).rgb;
    float3 NormalSample = g_NormalGBuffer.Sample(g_LinearWrapSampler, Input.UVs).rgb;
    float3 WorldPos = g_PositionGBuffer.Sample(g_LinearWrapSampler, Input.UVs).rgb;
	
	// DEBUG
	//
    Output.Result = float4(AlbedoSample, 1);
    return Output;

    float3 LightDir = -normalize(PointLights[0].Position.xyz - WorldPos);
    float Angle = max(dot(NormalSample, LightDir), 0);

    float Distance = length(PointLights[0].Position.xyz - WorldPos);
    float Attenuation = 1.0f / (Distance * Distance);
    float3 Radiance = (PointLights[0].Color.rgb * PointLights[0].Brightness) * Attenuation;

    Output.Result = float4(MatColor * Radiance, 1);

    return Output;
}


float3 WorldPosFromDepth(float _Depth, float2 _TexCoords)
{
    float Z = _Depth * 2.0f - 1.0f;

    float4 ClipSpacePos = float4(_TexCoords * 2.0f - 1.0f, Z, 1.0f);
    float4 ViewSpacePos = mul(InverseProjMat, ClipSpacePos);

    ViewSpacePos /= ViewSpacePos.w;

    float4 WorldSpacePos = mul(InverseViewMat, ViewSpacePos);

    return WorldSpacePos.xyz;
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * CameraNearZ * CameraFarZ) / (CameraFarZ + CameraNearZ - z * (CameraFarZ - CameraNearZ)) / CameraFarZ;
}