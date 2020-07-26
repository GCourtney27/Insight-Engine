#include <../Deferred_Rendering/Deferred_Rendering.hlsli>	

struct VS_INPUT_POSTFX
{
    float3 position : POSITION;
    float2 texCoords : TEXCOORD;
};

struct VS_OUTPUT_POSTFX
{
    float4 sv_position : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

VS_OUTPUT_POSTFX main(VS_INPUT_POSTFX vs_in)
{
    VS_OUTPUT_POSTFX vs_out;
	
    vs_out.sv_position = float4(vs_in.position, 1.0);
    vs_out.texCoords = vs_in.texCoords;
	
    return vs_out;
}
