#include "../../Public/Core/Core.hlsli"


// Structures
//
struct SP_VSInput
{
    float3 Position : POSITION;
};

struct SP_PSInput
{
    float4 Position : SV_POSITION;
    float3 UVs : TEXCOORDS;
};

//
// Entry Point
//
SP_PSInput main(SP_VSInput Input)
{
    SP_PSInput Output;
    
    matrix ViewNoMovement = ViewMat;
    ViewNoMovement._41 = 0;
    ViewNoMovement._42 = 0;
    ViewNoMovement._43 = 0;
    
    matrix ViewProjection = mul(ViewNoMovement, ProjMat);
    Output.Position = mul(float4(Input.Position, 1.f), ViewProjection);
    Output.Position.z = Output.Position.w;
    
    Output.UVs = Input.Position;
    
    return Output;
}
