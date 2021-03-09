#include "../Common/InsightCommon.hlsli"
#include "../ForwardRendering/ForwardRendering.hlsli"

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
};

struct VS_OUTPUT
{
    float4 sv_position : SV_POSITION;
    float3 FragPos : FRAG_POS;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
};


VS_OUTPUT main(VS_INPUT vs_in)
{
    VS_OUTPUT vs_out;
	
    matrix worldView = mul(world, view);
    float4x4 worldViewProjection = mul(mul(world, view), projection);
    float4 worldPos = mul(float4(vs_in.position, 1.0), world);
    
    vs_out.sv_position = mul(float4(vs_in.position, 1.0f), worldViewProjection);
	
    vs_out.FragPos = worldPos.xyz;
    vs_out.texCoords = float2((vs_in.texCoords.x + uvOffset.x) * tiling.x, (vs_in.texCoords.y + uvOffset.y) * tiling.y);
    
    vs_out.normal = normalize(mul(float4(vs_in.normal, 0.0f), world)).xyz;
    vs_out.tangent = mul(vs_in.tangent, (float3x3) worldView);
    vs_out.biTangent = mul(vs_in.biTangent, (float3x3) worldView);

    return vs_out;
}
