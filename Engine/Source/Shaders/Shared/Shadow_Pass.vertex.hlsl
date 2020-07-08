#include <../DeferredRendering/Deferred_Rendering.hlsli>	

// Entry Point
// -----------
VS_OUTPUT_SHADOWPASS main(VS_INPUT_SHADOWPASS vs_in)
{
    VS_OUTPUT_SHADOWPASS vs_out;
    
    //float4 worldPos = mul(float4(vs_in.position, 1.0), lightSpace);
    float4 worldPos = mul(float4(vs_in.position, 1.0), world);
    float4x4 lightSpaceVert = mul(world, lightSpace);
    float4x4 worldViewProjection = mul(mul(world, view), projection);
    
    vs_out.sv_position = mul(float4(vs_in.position, 1.0), worldViewProjection);
    vs_out.fragPos = worldPos.xyz;
	
    vs_out.texCoords = float2(0.0, 0.0);
    vs_out.normal = float3(0.0, 0.0, 0.0);
    vs_out.tangent = float3(0.0, 0.0, 0.0);
    vs_out.biTangent = float3(0.0, 0.0, 0.0);
    
    return vs_out;
}