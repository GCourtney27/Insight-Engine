#pragma pack_matrix(row_major)

cbuffer perObjectBuffer : register(b0) // Defined in ConstantBufferTypes
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
};

struct VS_INPUT // Defined in InitializeShaders() in Graphics.cpp with D3D11_INPUT_ELEMENT_DESC
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
};

struct VS_OUTPUT // What this shader returns to the pixel shader with VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outnormal : NORMAL;
    float3 outWorldPos : WORLD_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix);
    output.outTexCoord = input.inTexCoord;
    output.outnormal = normalize(mul(float4(input.inNormal, 0.0f), worldMatrix));
    output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix);

    return output;
}