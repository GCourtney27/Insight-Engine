#pragma pack_matrix(row_major)

cbuffer perObjectBuffer : register(b0) // Defined in ConstantBufferTypes
{
	float4x4 wvpMatrix;
	float4x4 worldMatrix;
};

struct SKY_VS_INPUT // Defined in InitializeShaders() in Graphics.cpp with D3D11_INPUT_ELEMENT_DESC
{
	float4 inPos : POSITION;
	float4 inNormal : NORMAL;
	float2 inTexCoord : TEXCOORD;
};

struct SKY_PS_OUTPUT // What this shader returns to the pixel shader with VS_OUTPUT
{
	float4 outPosition : SV_POSITION;
	float3 outTexCoord : TEXCOORD;
};

SKY_PS_OUTPUT main(SKY_VS_INPUT input)
{
	SKY_PS_OUTPUT output;

	output.outPosition = mul(input.inPos, wvpMatrix).xyww;
	//output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix).xyww;
	output.outTexCoord = input.inPos;

	return output;
}