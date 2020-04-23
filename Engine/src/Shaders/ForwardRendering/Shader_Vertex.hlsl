
struct VS_INPUT
{
    float4 position  : POSITION;
    float2 texCoords : TEXCOORD;
    float3 normal    : NORMAL;
};

struct VS_OUTPUT
{
    float4 position  : SV_POSITION;
    float2 texCoords : TEXCOORD;
    float3 normal    : NORMAL;
};

cbuffer cbPerObject : register(b0)
{
    float4x4 wvpMat;
};

VS_OUTPUT main(VS_INPUT vs_in)
{
    VS_OUTPUT vs_out;

    vs_out.position = mul(vs_in.position, wvpMat);
    vs_out.texCoords = vs_in.texCoords;
    vs_out.normal = vs_in.normal;

    return vs_out;
}