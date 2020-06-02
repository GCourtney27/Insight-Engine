
cbuffer cbPerObject : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
};

struct VS_INPUT
{
    float3 inPosition : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBiTangent : BITANGENT;
};

struct PS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outTexCoord : TEXCOORD;
};

// Entry Point
// -----------
PS_OUTPUT main(VS_INPUT input)
{
    PS_OUTPUT output;
    matrix worldViewProj = mul(mul(world, view), projection);

    output.outPosition = mul(input.inPosition, worldViewProj).xyww;
	//output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix).xyww;
    output.outTexCoord = input.inPosition;

    return output;
}
