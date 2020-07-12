#include <../DeferredRendering/Deferred_Rendering.hlsli>	

// Entry Point
// -----------
VS_OUTPUT_SHADOWPASS main(VS_INPUT_SHADOWPASS vs_in)
{
    VS_OUTPUT_SHADOWPASS vs_out;
    
    //vs_in.position.x *= 0.7;
    //vs_in.position.y *= 0.7;
    //vs_in.position.z *= 0.7;
    
    float4 worldPos = mul(float4(vs_in.position, 1.0), world);
    float4x4 wvpLightSpace = mul(mul(world, lightSpaceView), lightSpaceProj);
    
    float4x4 mat;
    
    
    vs_out.sv_position = mul(float4(vs_in.position, 1.0), wvpLightSpace);
    vs_out.fragPos = worldPos.xyz;
	
    vs_out.texCoords = float2(0.0, 0.0);
    vs_out.normal = float3(0.0, 0.0, 0.0);
    vs_out.tangent = float3(0.0, 0.0, 0.0);
    vs_out.biTangent = float3(0.0, 0.0, 0.0);
    
    return vs_out;
}