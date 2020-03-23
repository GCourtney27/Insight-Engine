#pragma pack_matrix(row_major)

cbuffer perObjectBuffer : register(b0) // Defined in ConstantBufferTypes
{
	float4x4 wvpMatrix;
	float4x4 worldMatrix;
};

struct VS_INPUT // Defined in InitializeShaders() in Graphics.cpp with D3D11_INPUT_ELEMENT_DESC
{
	float3 inPos : POSITION;
	float2 inTexCoord_bc : BASECOLOR;
	float2 inTexCoord_o : OPACITY;
	float3 inNormal : NORMAL;
};

struct VS_OUTPUT // What this shader returns to the pixel shader with VS_OUTPUT
{
	float4 outPosition : SV_POSITION;
	float2 outTexCoord_bc : BASECOLOR;
	float2 outTexCoord_o : OPACITY;
	float3 outnormal : NORMAL;
	float3 outWorldPos : WORLD_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix);
	output.outTexCoord_bc = input.inTexCoord_bc;
	output.outTexCoord_o = input.inTexCoord_o;
	output.outnormal = normalize(mul(float4(input.inNormal, 0.0f), worldMatrix));
	output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix);

	return output;
}