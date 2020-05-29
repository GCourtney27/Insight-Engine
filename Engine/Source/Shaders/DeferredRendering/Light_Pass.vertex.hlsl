#include <Deferred_Rendering.hlsli>	



VS_OUTPUT_LIGHTPASS main(VS_INPUT_LIGHTPASS vs_in)
{
    VS_OUTPUT_LIGHTPASS vs_out;
	
	vs_out.sv_position = float4(vs_in.position, 1.0);
	vs_out.texCoords = vs_in.texCoords;
	
	return vs_out;
}
