
struct PS_IN
{
    float4 sv_position : SV_POSITION;
    float2 TexCoords : TEXCOORDS;
};

Texture2D t_Texture : register(t0);
SamplerState s_LinearWrapSampler : register(s0);

float4 main(PS_IN ps_in) : SV_TARGET
{
    float3 color = t_Texture.Sample(s_LinearWrapSampler, ps_in.TexCoords).rgb;
    
    return float4(color, 1.0f);
}