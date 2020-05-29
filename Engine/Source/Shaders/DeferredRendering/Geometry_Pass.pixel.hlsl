#include <Deferred_Rendering.hlsli>	

//Texture2D t_Albedo : register(t0);
//Texture2D t_Normal : register(t1);
//
//SamplerState s_Sampler : register(s0);

PS_OUTPUT_GEOMPASS main(PS_INPUT_GEOMPASS ps_in)
{
	PS_OUTPUT_GEOMPASS ps_out;
	
	ps_out.diffuse = float4(1.0, 0.0, 0.0, 1.0);//t_Albedo.Sample(s_Sampler, ps_in.texCoords);
	ps_out.normal.rgb = ps_in.normal;
	ps_out.position = ps_in.position;
	
	return ps_out;
}