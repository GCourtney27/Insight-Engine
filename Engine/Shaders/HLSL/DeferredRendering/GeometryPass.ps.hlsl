#include "../Common/InsightCommon.hlsli"
#include "DeferredRendering.hlsli"	

// Per-Object Texture Inputs
// --------------
Texture2D t_AlbedoObject    : register(t5);
Texture2D t_NormalObject    : register(t6);
Texture2D t_RougnessObject  : register(t7);
Texture2D t_MetallicObject  : register(t8);
Texture2D t_AOObject        : register(t9);


// Samplers
// --------
SamplerState s_LinearWrapSampler : register(s1);

// Entry Point
// -----------
PS_OUTPUT_GEOMPASS main(PS_INPUT_GEOMPASS ps_in)
{
    PS_OUTPUT_GEOMPASS ps_out;
    
    float3 normalSample = t_NormalObject.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    
    const float3x3 tanToView = float3x3(normalize(ps_in.tangent),
                                        normalize(ps_in.biTangent),
                                        normalize(ps_in.normal));
    
    float3 normal;
    normal.x =  normalSample.x * 2.0f - 1.0f;
    normal.y = -normalSample.y * 2.0f + 1.0f;
    normal.z =  normalSample.z;
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