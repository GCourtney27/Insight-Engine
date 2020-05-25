
Texture2D t_Albedo : register(t0);
Texture2D t_Normal : register(t1);

SamplerState s_Sampler : register(s0);

struct PS_OUT_DEFERRED
{
	float4 diffuse : SV_Target0;
	float4 normal : SV_Target1;
	float4 position : SV_Target2;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 FragPos : FRAG_POS;
	float2 texCoords : TEXCOORD;
	float3 normal : NORMAL;
};

PS_OUT_DEFERRED main(PS_INPUT ps_in)
{
	PS_OUT_DEFERRED ps_out;
	
	ps_out.diffuse = t_Albedo.Sample(s_Sampler, ps_in.texCoords);
	ps_out.normal = ps_in.normal;
	ps_out.position = ps_in.position;
	
	return ps_out;
}