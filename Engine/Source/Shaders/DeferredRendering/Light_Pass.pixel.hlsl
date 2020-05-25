
Texture2D t_Diffuse : register(t0);
Texture2D t_Normal : register(t1);
Texture2D t_Position : register(t2);

SamplerState s_Sampler : register(s0);

struct PS_INPUT
{
	float3 sv_position : SV_POSITION;
	float2 texCoords : TEXCOORD;
};


float4 main(PS_INPUT ps_in) : SV_TARGET
{
	float3 result = float3(0.0, 0.0, 0.0);
	
	float3 albedoSample		= t_Diffuse.Sample(s_Sampler, ps_in.texCoords).rgb;
	float3 normalSample		= t_Normal.Sample(s_Sampler, ps_in.texCoords).rgb;
	float3 positionSample	= t_Position.Sample(s_Sampler, ps_in.texCoords).rgb;
	
	result = float3(1.0, 0.0, 0.0);
	
	return float4(result, 1.0);
}
