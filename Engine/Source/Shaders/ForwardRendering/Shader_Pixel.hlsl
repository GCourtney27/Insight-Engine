
Texture2D tAlbedo : register(t0);
Texture2D tNormal : register(t1);

SamplerState sampler1 : register(s0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

cbuffer cbPerFrame :register(b1)
{
    float3 cameraPosition;
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
  /*  float depth = ps_in.position.z / ps_in.position.w;
    return float4(depth, depth, depth, 1.0);*/
    float4 albedo = tAlbedo.Sample(sampler1, ps_in.texCoords);
    float4 normal = tNormal.Sample(sampler1, ps_in.texCoords);
    float3 color = lerp(albedo, normal, 0.5);
    return albedo;
    //return float4(cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f);
    //float4 color = {1.0f, 0.0f, 0.0f, 1.0f};
    /*float4 color = {ps_in.normal, 1.0f};
    return color;*/

}