#include <Deferred_Rendering.hlsli>	

//Texture2D t_Albedo : register(t0);
//Texture2D t_Normal : register(t1);
//
//SamplerState s_Sampler : register(s0);

struct PS_OUTPUT_GEOMPASS
{
    float3 diffuse	: SV_Target0;
    float4 normal	: SV_Target1;
    float4 position : SV_Target2;
};

PS_OUTPUT_GEOMPASS main(PS_INPUT_GEOMPASS ps_in) : SV_TARGET
{
	PS_OUTPUT_GEOMPASS ps_out;
	
	ps_out.diffuse = float3(0.0, 1.0, 0.0);//t_Albedo.Sample(s_Sampler, ps_in.texCoords);
    ps_out.normal = float4(ps_in.normal, 1.0);
	ps_out.position = ps_in.position;
	
	return ps_out;
}