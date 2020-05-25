
struct VS_INPUT
{
	float3 position	 : POSITION;
	float2 texCoords : TEXCOORD;
};

struct VS_OUTPUT
{
	float3 sv_position	 : SV_POSITION;
	float2 texCoords : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT vs_in)
{
	VS_OUTPUT vs_out;
	
	vs_out.sv_position = vs_in.position;
	vs_out.texCoords = vs_in.texCoords;
	
	return vs_out;
}
