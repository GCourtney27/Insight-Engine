#include "../DeferredRendering/DeferredRendering.hlsli"

struct PS_OUTPUT_SHADOWPASS
{
    float Depth : SV_Depth;
};

// Entry Point
// -----------
PS_OUTPUT_SHADOWPASS main(PS_INPUT_SHADOWPASS ps_in)
{
    PS_OUTPUT_SHADOWPASS ps_out;
    
    ps_out.Depth = ps_in.sv_position.z;
    
    return ps_out;
}