
Texture2D<float4> t_Source : register(t0);
RWTexture2D<float4> t_Destination: register(u0);

cbuffer ThreshholdParams : register(b0)
{
    float cbThreshold;
}

[numthreads(8, 8, 1)]
void main(uint3 GroupID : SV_GroupID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex, uint3 DispatchID : SV_DispatchThreadID)
{
    uint2 PixelPos = uint2(DispatchID.x, DispatchID.y);
    
    uint2 inPixel = PixelPos * 2.0;
    float4 hIntensity0 = lerp(t_Source[inPixel], t_Source[inPixel + uint2(1, 0)], 0.5);
    float4 hIntensity1 = lerp(t_Source[inPixel + uint2(0, 1)], t_Source[inPixel + uint2(1, 1)], 0.5);
    float4 intensity = lerp(hIntensity0, hIntensity1, 0.5);
    
    float intensityTest = (float)(length(intensity.rgb) > cbThreshold);
    
    t_Destination[PixelPos] = float4(intensityTest * intensity.rgb, 1.0);
}
