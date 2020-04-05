
struct VS_INPUT
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 wvpMat;
};

VS_OUTPUT main(VS_INPUT vs_in)
{
    VS_OUTPUT vs_out;

    vs_out.position = mul(vs_in.position, wvpMat);
    vs_out.color = vs_in.color;
    
    return vs_out;
}