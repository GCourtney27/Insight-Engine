
struct PS_IN
{
    float4 sv_position : SV_POSITION;
    float2 TexCoords : TEXCOORDS;
    float3 FragPos : FRAGPOS;
};

Texture2D t_Albedo : register(t0);
Texture2D t_Normal : register(t1);
Texture2D t_Roughness : register(t2);
Texture2D t_Metallic : register(t3);
Texture2D t_AO : register(t4);

TextureCube t_IR : register(t5);
TextureCube t_EnvMap : register(t6);

SamplerState s_LinearWrapSampler : register(s0);

float4 main(PS_IN ps_in) : SV_TARGET
{
    float3 color = t_Albedo.Sample(s_LinearWrapSampler, ps_in.TexCoords).rgb;
    
    return float4(color, 1.0f);
}