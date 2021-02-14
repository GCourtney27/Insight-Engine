#include "Shaders/HLSL/Common/InsightCommon.h"
#include "Shaders/HLSL/DeferredRendering/DeferredRendering.h"	

// Entry Point
// -----------
VS_OUTPUT_GEOMPASS main(VS_INPUT_GEOMPASS vs_in)
{
	VS_OUTPUT_GEOMPASS vs_out;
	
    matrix worldView                = mul(cbWorld, cbView);
    float4x4 worldViewProjection    = mul(mul(cbWorld, cbView), cbProjection);
    float4 worldPos                 = mul(float4(vs_in.position, 1.0), cbWorld);
    
    vs_out.sv_position = mul(float4(vs_in.position, 1.0f), worldViewProjection);
	
    vs_out.fragPos = worldPos.xyz;
    
    vs_out.texCoords    = float2((vs_in.texCoords.x + uvOffset.x) * tiling.x, (vs_in.texCoords.y + uvOffset.y) * tiling.y);
    
    vs_out.normal       = normalize(mul(float4(vs_in.normal, 0.0f), cbWorld).xyz);
    vs_out.tangent      = mul(float4(vs_in.tangent, 1.0), worldView).xyz;
    vs_out.biTangent    = mul(float4(vs_in.biTangent, 1.0), worldView).xyz;

	return vs_out;
}
