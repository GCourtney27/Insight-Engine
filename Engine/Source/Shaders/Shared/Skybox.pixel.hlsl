struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float3 inTexCoord : TEXCOORD;
};

TextureCube t_SkyMap : register(t13);

SamplerState s_LinearWrapSampler : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 diffuse = t_SkyMap.Sample(s_LinearWrapSampler, input.inTexCoord).rgb;
    return float4(diffuse, 1.0);
}