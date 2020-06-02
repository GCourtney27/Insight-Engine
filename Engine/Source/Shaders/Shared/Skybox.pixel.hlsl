struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float3 inTexCoord : TEXCOORD;
};

TextureCube t_SkyMap : register(t0);

SamplerState s_LinearWrapSampler : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 result = t_SkyMap.Sample(s_LinearWrapSampler, input.inTexCoord);
	//float4 res = {1.0f, 1.0f, 0.0f, 1.0f};
    return result;
}