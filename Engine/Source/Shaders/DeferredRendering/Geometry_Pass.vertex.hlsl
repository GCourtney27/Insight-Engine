#include <Deferred_Rendering.hlsli>	

VS_OUTPUT_GEOMPASS main(VS_INPUT_GEOMPASS vs_in)
{
	VS_OUTPUT_GEOMPASS vs_out;
	
	matrix worldView = mul(world, view);
	float4x4 worldViewProjection = mul(mul(world, view), projection);
	
	vs_out.position = mul(float4(vs_in.position, 1.0f), worldViewProjection);
	
	vs_out.FragPos = float3(mul(world, float4(vs_in.position, 1.0)).xyz);
	vs_out.texCoords = vs_in.texCoords;
	
    vs_out.normal = vs_in.normal;//normalize(mul(float4(vs_in.normal, 0.0f), world));

	return vs_out;
}
