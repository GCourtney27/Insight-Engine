#include <Deferred_Rendering.hlsli>	

Texture2D t_AlbedoObject : register(t4);
Texture2D t_NormalObject : register(t5);
Texture2D t_SpecularObject : register(t6);
//Texture2D t_Metallic : register(t7);
//Texture2D t_Roughness : register(t8);
//Texture2D t_AO : register(t9);

SamplerState s_LinearWrapSampler : register(s1);

struct PS_OUTPUT_GEOMPASS
{
    float3 albedo : SV_Target0;
    float4 normal   : SV_Target1;
    float4 position : SV_Target2;
};

PS_OUTPUT_GEOMPASS main(PS_INPUT_GEOMPASS ps_in) : SV_TARGET
{
    PS_OUTPUT_GEOMPASS ps_out;
    
    float3 normalSample = t_NormalObject.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    
    const float3x3 tanToView = float3x3(normalize(ps_in.tangent),
                                        normalize(ps_in.biTangent),
                                        normalize(ps_in.normal));
    
    //float3 normal; // TODO Replace ps_in.normal with t_Normal.Sample(s_LinearWrapSampler , ps_in.texCoords);
    //normal.x = normalSample.x * 2.0f - 1.0f;
    //normal.y = normalSample.y * 2.0f + 1.0f;
    //normal.z = normalSample.z;
    //normal = normalize(mul(normal, tanToView));
    
    ps_out.albedo = t_AlbedoObject.Sample(s_LinearWrapSampler, ps_in.texCoords);
    ps_out.normal = float4(normalSample, 1.0);
    ps_out.position = float4(ps_in.fragPos, 1.0);
	
    return ps_out;
}