#include <../Common/Insight_Common.hlsli>
#include <Deferred_Rendering.hlsli>	

// Entry Point
// -----------
VS_OUTPUT_GEOMPASS main(VS_INPUT_GEOMPASS vs_in)
{
	VS_OUTPUT_GEOMPASS vs_out;
	
    matrix worldView = mul(world, cbView);
    float4x4 worldViewProjection = mul(mul(world, cbView), cbProjection);
    float4 worldPos = mul(float4(vs_in.position, 1.0), world);
    
    vs_out.sv_position = mul(float4(vs_in.position, 1.0f), worldViewProjection);
	
    vs_out.fragPos = worldPos.xyz;
    vs_out.texCoords = float2((vs_in.texCoords.x + uvOffset.x) * tiling.x, (vs_in.texCoords.y + uvOffset.y) * tiling.y);
    
    vs_out.normal = normalize(mul(float4(vs_in.normal, 0.0f), world)).xyz;
    vs_out.tangent = mul(vs_in.tangent, (float3x3) worldView);
    vs_out.biTangent = mul(vs_in.biTangent, (float3x3) worldView);

	return vs_out;
}
