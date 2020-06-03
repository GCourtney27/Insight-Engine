#include <Deferred_Rendering.hlsli>	

// Texture Inputs
// --------------
Texture2D t_AlbedoObject    : register(t5);
Texture2D t_NormalObject    : register(t6);
Texture2D t_RougnessObject  : register(t7);
Texture2D t_MetallicObject  : register(t8);
Texture2D t_AOObject        : register(t9);


// Samplers
// --------
SamplerState s_LinearWrapSampler : register(s1);

struct PS_OUTPUT_GEOMPASS
{
    float3 albedo               : SV_Target0;
    float4 normal               : SV_Target1;
    float3 roughnessMetallicAO  : SV_Target2;
    float4 position             : SV_Target3;
};

// Entry Point
// -----------
PS_OUTPUT_GEOMPASS main(PS_INPUT_GEOMPASS ps_in) : SV_TARGET
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
    
    ps_out.normal = float4(normal, 1.0);
    ps_out.position = float4(ps_in.fragPos, 1.0);
    ps_out.albedo = t_AlbedoObject.Sample(s_LinearWrapSampler, ps_in.texCoords);
    ps_out.roughnessMetallicAO.b = t_AOObject.Sample(s_LinearWrapSampler, ps_in.texCoords).r;
    ps_out.roughnessMetallicAO.r = t_RougnessObject.Sample(s_LinearWrapSampler, ps_in.texCoords).r;
    ps_out.roughnessMetallicAO.g = t_MetallicObject.Sample(s_LinearWrapSampler, ps_in.texCoords).r;
	
    return ps_out;
}