#include "../Core/Core.hlsli"
#include "DeferredShadingCommon.hlsli"


//
// Entry Point
//
LP_PSInput main(LP_VSInput Input)
{
    LP_PSInput Output;

    Output.Position = float4(Input.Position, 0, 1);
    Output.UVs = Input.UVs;

    return Output;
}
