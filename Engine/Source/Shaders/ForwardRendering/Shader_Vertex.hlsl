
struct VS_INPUT
{
	float3 position  : POSITION;
	float2 texCoords : TEXCOORD;
	float3 normal    : NORMAL;
	float3 tangent   : TANGENT;
	float3 biTangent : BITANGENT;

	float3 instPosition  : INSTANCE_POSITION;
};

struct VS_OUTPUT
{
	float4 position  : SV_POSITION;
	float2 texCoords : TEXCOORD;
	float3 normal    : NORMAL;
	float3 tangent   : TANGENT;
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
};

VS_OUTPUT main(VS_INPUT vs_in)
{
	VS_OUTPUT vs_out;

	float4 pos = float4(vs_in.position, 1.0f);

	pos.x += vs_in.instPosition.x;
	pos.y += vs_in.instPosition.y;
	pos.z += vs_in.instPosition.z;

	pos = mul(pos, world);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	vs_out.position = pos;
	
	vs_out.texCoords = vs_in.texCoords;
	vs_out.normal = vs_in.normal;
	vs_out.tangent = vs_in.tangent;
	vs_out.biTangent = vs_in.biTangent;

	return vs_out;
}