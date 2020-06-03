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
    
    diffuse = diffuse / (diffuse + float3(1.0, 1.0, 1.0));
    float3 gammaCorrect = lerp(diffuse, pow(diffuse, 1.0 / 2.2), 0.4f);
    
    return float4(gammaCorrect, 1.0);
}