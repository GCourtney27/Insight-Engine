#include <Deferred_Rendering.hlsli>	

Texture2D t_Albedo : register(t4);
Texture2D t_Normal : register(t5);
Texture2D t_Specular : register(t6);
//Texture2D t_Metallic : register(t7);
//Texture2D t_Roughness : register(t8);
//Texture2D t_AO : register(t9);

SamplerState s_Sampler : register(s1);

struct PS_OUTPUT_GEOMPASS
{
    float3 albedo : SV_Target0;
    float4 normal   : SV_Target1;
    float4 position : SV_Target2;
};

PS_OUTPUT_GEOMPASS main(PS_INPUT_GEOMPASS ps_in) : SV_TARGET
{
    PS_OUTPUT_GEOMPASS ps_out;
    
    const float3x3 tanToView = float3x3(normalize(ps_in.tangent),
                                        normalize(ps_in.biTangent),
                                        normalize(ps_in.normal));
    float3 normal; // TODO Replace ps_in.normal with t_Normal.Sample(s_Sampler, ps_in.texCoords);
    normal.x =  ps_in.normal.x * 2.0f - 1.0f;
    normal.y = -ps_in.normal.y * 2.0f + 1.0f;
    normal.z =  ps_in.normal.z;
    normal = normalize(mul(normal, tanToView));
    
    ps_out.albedo = float3(0.0, 1.0, 0.0); //t_Albedo.Sample(s_Sampler, ps_in.texCoords);
    ps_out.normal = float4(ps_in.normal, 1.0);
    ps_out.position = float4(ps_in.fragPos, 1.0);
	
    return ps_out;
}