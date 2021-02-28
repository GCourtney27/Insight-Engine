#include "Shaders/HLSL/DeferredRendering/DeferredRendering.h"

////////////////////////////
// Common In-Out Structures
////////////////////////////
struct VSInput
{
    float3 position     : POSITION;
    float2 texCoords    : TEXCOORD;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 biTangent    : BITANGENT;
};
struct VSOutput
{
    float4 sv_position  : SV_POSITION;
    float3 fragPos      : FRAG_POS;
    float2 texCoords    : TEXCOORD;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 biTangent    : BITANGENT;
};
struct PSOutput
{
    float3 albedo                   : SV_Target0;
    float4 normal                   : SV_Target1;
    float4 roughnessMetallicAOSpec  : SV_Target2;
    float4 position                 : SV_Target3;
};



///////////////////
// Vertex Shader
///////////////////

// Entry Point
// -----------
VSOutput VSmain(VSInput vs_in)
{
    VSOutput vs_out;
	
    matrix worldView = mul(cbWorld, cbView);
    float4x4 worldViewProjection = mul(mul(cbWorld, cbView), cbProjection);
    float4 worldPos = mul(float4(vs_in.position, 1.0), cbWorld);
    
    vs_out.sv_position = mul(float4(vs_in.position, 1.0f), worldViewProjection);
	
    vs_out.fragPos = worldPos.xyz;
    
    vs_out.texCoords = float2((vs_in.texCoords.x + uvOffset.x) * tiling.x, (vs_in.texCoords.y + uvOffset.y) * tiling.y);
    
    vs_out.normal = normalize(mul(float4(vs_in.normal, 0.0f), cbWorld).xyz);
    vs_out.tangent = mul(float4(vs_in.tangent, 1.0), worldView).xyz;
    vs_out.biTangent = mul(float4(vs_in.biTangent, 1.0), worldView).xyz;

    return vs_out;
}



//////////////////
// Pixel Shader
//////////////////

// Per-Object Texture Inputs
// --------------
Texture2D t_AlbedoObject : register(t5);
Texture2D t_NormalObject : register(t6);
Texture2D t_RougnessObject : register(t7);
Texture2D t_MetallicObject : register(t8);
Texture2D t_AOObject : register(t9);


// Samplers
// --------
SamplerState s_LinearWrapSampler : register(s1);

// Entry Point
// -----------
PSOutput PSmain(VSOutput ps_in)
{
    PSOutput ps_out;
    
    float3 normalSample = t_NormalObject.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    
    const float3x3 tanToView = float3x3(normalize(ps_in.tangent),
                                        normalize(ps_in.biTangent),
                                        normalize(ps_in.normal));
    
    float3 normal;
    normal.x = normalSample.x * 2.0f - 1.0f;
    normal.y = -normalSample.y * 2.0f + 1.0f;
    normal.z = normalSample.z;
    normal = normalize(mul(normal, tanToView));
    
    ps_out.normal = float4(normal, 1.0); // Tangent space
    ps_out.position = float4(ps_in.fragPos, 1.0);
    
    ps_out.albedo = t_AlbedoObject.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb + diffuseAdditive;
    ps_out.roughnessMetallicAOSpec.r = t_RougnessObject.Sample(s_LinearWrapSampler, ps_in.texCoords).r + roughnessAdditive;
    ps_out.roughnessMetallicAOSpec.g = t_MetallicObject.Sample(s_LinearWrapSampler, ps_in.texCoords).r + metallicAdditive;
    ps_out.roughnessMetallicAOSpec.b = t_AOObject.Sample(s_LinearWrapSampler, ps_in.texCoords).r;
    ps_out.roughnessMetallicAOSpec.a = specular;
    
    return ps_out;
}
