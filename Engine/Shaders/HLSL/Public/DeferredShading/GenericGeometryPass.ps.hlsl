#include "../Core/Core.hlsli"
#include "DeferredShadingCommon.hlsli"


// Samplers
//
SamplerState g_LinearWrapSampler : register(s0);

// Opaque material Textures
//
Texture2D g_Albedo : register(t0);
Texture2D g_Normal : register(t1);

//
// Entry Point
//
GP_PSOutput main(GP_PSInput Input)
{
    GP_PSOutput Output;

    float3 AlbedoSample = g_Albedo.Sample(g_LinearWrapSampler, Input.UVs).rgb;
    float3 NormalSample = g_Normal.Sample(g_LinearWrapSampler, Input.UVs).rgb;

    const float3x3 TangentToView = float3x3(normalize(Input.Tangent),
											normalize(Input.BiTangent),
													  Input.Normal);
    float3 NormalTangentSpace;
    NormalTangentSpace.x =  NormalSample.x * 2.0f - 1.0f;
    NormalTangentSpace.y = -NormalSample.y * 2.0f + 1.0f;
    NormalTangentSpace.z =  NormalSample.z;
    NormalTangentSpace = normalize(mul(NormalTangentSpace, TangentToView));

    Output.Normal = float4(NormalTangentSpace, 1);
    Output.Albedo = float4(AlbedoSample, 1) + MatColor;
    Output.Position = float4(Input.WorldPos, 1);
	
    return Output;
}