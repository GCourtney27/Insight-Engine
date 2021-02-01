#include "RayTraceCommon.hlsli"

[shader("miss")]
void Miss(inout HitInfo HitInfo : SV_RayPayload)
{
    // If nothing is hit just return black. The result will be added to the result
    // of the light pass in the post-process shader. So, evectivly, we will just
    // take the result of the light pass.
    HitInfo.ColorAndDistance = float4(1.0f, 1.0f, 1.0f, -1.f);
}