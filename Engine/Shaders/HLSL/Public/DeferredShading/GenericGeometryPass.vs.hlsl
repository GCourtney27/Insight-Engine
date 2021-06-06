#include "../Core/Core.hlsli"
#include "DeferredShadingCommon.hlsli"


//
// Entry Point
//
GP_PSInput main(GP_VSInput Input)
{
    GP_PSInput Result;

    float4x4 WorldView = mul(WorldMat, ViewMat);
    float4x4 WorldViewProjection = mul(WorldView, ProjMat);
    
    Result.Position = mul(float4(Input.Position, 1.0f), WorldViewProjection);
    Result.WorldPos = mul(float4(Input.Position, 1.0f), WorldMat).xyz;
	
    Result.VertexColor = Input.Color;
    Result.UVs = Input.UVs;

    Result.Normal = normalize(mul(float4(Input.Normal, 0), WorldMat)).xyz;
    Result.Tangent = mul(float4(Input.Tangent, 1), WorldView).xyz;
    Result.BiTangent = mul(float4(Input.BiTangent, 1), WorldView).xyz;

    return Result;
}