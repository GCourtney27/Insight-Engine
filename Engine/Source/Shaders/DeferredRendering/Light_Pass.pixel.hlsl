#include <Deferred_Rendering.hlsli>	

Texture2D t_Diffuse		: register(t0);
Texture2D t_Normal		: register(t1);
Texture2D t_Position	: register(t2);
Texture2D t_Depth		: register(t3);

sampler s_Sampler : register(s0);

float4 main(PS_INPUT_LIGHTPASS ps_in) : SV_TARGET
{
	float3 result = float3(0.0, 0.0, 0.0);
	
	float3 diffuseSample		= t_Diffuse.Sample(s_Sampler, ps_in.texCoords).rgb;
	float3 normalSample		= t_Normal.Sample(s_Sampler, ps_in.texCoords).rgb;
	float3 positionSample	= t_Position.Sample(s_Sampler, ps_in.texCoords).rgb;
	
    result = float3(diffuseSample);
	
    return float4(result, 1.0);
}
