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

    // Sample G-Buffer.
    float3 AlbedoSample = g_AlbedoGBuffer.Sample(g_LinearWrapSampler, Input.UVs).rgb;
    float3 NormalSample = g_NormalGBuffer.Sample(g_LinearWrapSampler, Input.UVs).rgb;
    float3 WorldPos     = g_PositionGBuffer.Sample(g_LinearWrapSampler, Input.UVs).rgb;
	
    // Acummulate point light luminance.
    float3 PointLightLuminance = float3(0.f, 0.f, 0.f);
    for (int i = 0; i < NumPointLights; i++)
    {
        float3 LightDir = -normalize(PointLights[i].Position.xyz - WorldPos);
        float Angle = max(dot(NormalSample, LightDir), 0);

        float Distance = length(PointLights[i].Position.xyz - WorldPos);
        float Attenuation = 1.0f / (Distance * Distance);
        PointLightLuminance += ((PointLights[i].Color.rgb * PointLights[i].Brightness) * Attenuation) * Angle;
    }
    
    // Accumulate directional light luminance.
    float3 DirectionalLightLuminance = float3(0.f, 0.f, 0.f);
    for (int d = 0; d < NumDirectionalLights; d++)
    {
        float3 LightDir = normalize(DirectionalLights[d].Direction.xyz);
        float Angle     = max(dot(NormalSample, LightDir), 0);
        
        DirectionalLightLuminance += (DirectionalLights[d].Color.rgb * DirectionalLights[d].Brightness) * Angle;
    }
        
    float3 LightLuminance = /*PointLightLuminance + */DirectionalLightLuminance;
    
    Output.Result = float4(AlbedoSample * LightLuminance, 1);
    
	// DEBUG
	//
    //Output.Result = float4(Radiance, 1);
    
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