
Texture2D tAlbedo : register(t0);
Texture2D tNormal : register(t1);

SamplerState sampler1 : register(s0);

struct PS_INPUT
{
    float4 position  : SV_POSITION;
    float2 texCoords : TEXCOORD;
    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
    float3 biTangent : BITANGENT;
};

cbuffer cbPerFrame :register(b1)
{
    float3 cameraPosition;
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float4 albedo = tAlbedo.Sample(sampler1, ps_in.texCoords);
    float4 normal = tNormal.Sample(sampler1, ps_in.texCoords);
    //return albedo;
    
    float4 color = {ps_in.normal, 1.0f};
    return color;

}