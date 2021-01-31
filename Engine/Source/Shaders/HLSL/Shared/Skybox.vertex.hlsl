#include <../DeferredRendering/DeferredRendering.hlsli>	

struct VS_INPUT
{
    float3 inPosition : POSITION;
    float2 inTexCoord : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outTexCoord : TEXCOORD;
};

// Entry Point
// -----------
PS_OUTPUT main(VS_INPUT input)
{
    PS_OUTPUT output;
    matrix viewNoMovement = cbView;
    viewNoMovement._41 = 0;
    viewNoMovement._42 = 0;
    viewNoMovement._43 = 0;
    
    matrix viewProjection = mul(viewNoMovement, cbProjection);
    output.outPosition = mul(float4(input.inPosition, 1.0), viewProjection);
    output.outPosition.z = output.outPosition.w;
    
    output.outTexCoord = input.inPosition;

    return output;
}
