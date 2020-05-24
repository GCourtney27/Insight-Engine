
struct VS_INPUT
{
	float3 position  : POSITION;
	float2 texCoords : TEXCOORD;
	float3 normal    : NORMAL;
	float3 tangent   : TANGENT;
	float3 biTangent : BITANGENT;
};

struct VS_OUTPUT
{
	float4 position  : SV_POSITION;
	float3 FragPos	 : FRAG_POS;
	float2 texCoords : TEXCOORD;
	float3 normal	 : NORMAL;
	float3 tangent	 : TANGENT;
	float3 biTangent : BITANGENT;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
};

cbuffer cbPerFrame : register(b1)
{
	float3 cameraPosition;
	float deltaMs;
	float time;
};

VS_OUTPUT main(VS_INPUT vs_in)
{
	VS_OUTPUT vs_out;
	
	matrix worldView = mul(world, view);
	float4x4 worldViewProjection = mul(mul(world, view), projection);
	
	vs_out.position = mul(float4(vs_in.position, 1.0f), worldViewProjection);
	
	vs_out.FragPos = float3(mul(world, float4(vs_in.position, 1.0)).xyz);
	vs_out.texCoords = vs_in.texCoords;
	
	vs_out.normal = normalize(mul(float4(vs_in.normal, 0.0f), world));
	vs_out.tangent = mul(vs_in.tangent, (float3x3) worldView);
	vs_out.biTangent = mul(vs_in.biTangent, (float3x3) worldView);

	return vs_out;
}
