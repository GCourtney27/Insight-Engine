Texture2D texture1 : register(t0);
SamplerState sampler1 : register(s0);

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

float4 main(VS_OUTPUT ps_in) : SV_TARGET
{
    //float depth = ps_in.position.z / ps_in.position.w;
    //return float4(depth, depth, depth, 1.0);
    return texture1.Sample(sampler1, ps_in.texCoord);
}