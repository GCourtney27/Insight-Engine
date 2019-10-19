#pragma pack_matrix(row_major)

cbuffer perObjectBuffer : register(b0) // Defined in ConstantBufferTypes
{
    matrix world;
    matrix view;
    matrix projection;
};

struct SKY_VS_INPUT // Defined in InitializeShaders() in Graphics.cpp with D3D11_INPUT_ELEMENT_DESC
{
    float3 inPosition : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBiTangent : BITANGENT;
};

struct SKY_PS_OUTPUT // What this shader returns to the pixel shader with VS_OUTPUT
{
	float4 outPosition : SV_POSITION;
	float3 outTexCoord : TEXCOORD;
};

SKY_PS_OUTPUT main(SKY_VS_INPUT input)
{
	SKY_PS_OUTPUT output;
    matrix worldViewProj = mul(mul(world, view), projection);

	output.outPosition = mul(input.inPosition, worldViewProj).xyww;
	//output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix).xyww;
	output.outTexCoord = input.inPosition;

	return output;
}