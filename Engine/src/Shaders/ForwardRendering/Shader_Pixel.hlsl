
Texture2D tAlbedo : register(t0);

SamplerState sampler1 : register(s0);

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
};

float4 main(VS_OUTPUT ps_in) : SV_TARGET
{
    /*float depth = ps_in.position.z / ps_in.position.w;
    return float4(depth, depth, depth, 1.0);*/

    //return tAlbedo.Sample(sampler1, ps_in.texCoords);
    //float4 color = {1.0f, 0.0f, 0.0f, 1.0f};
    float4 color = {ps_in.normal, 1.0f};
    return color;

}