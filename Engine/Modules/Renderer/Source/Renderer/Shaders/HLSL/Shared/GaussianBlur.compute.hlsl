#define GAUSSIAN_RADIUS 7

Texture2D<float4> t_Source : register(t0);
RWTexture2D<float4> t_Destination : register(u0);

cbuffer BlurParams : register(b0)
{
    float4 Coefficients[(GAUSSIAN_RADIUS + 1) / 4];
    int2 RadiusAndDirection;
}

[numthreads(8, 8, 1)]
void main(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    int2 pixel = int2(dispatchID.x, dispatchID.y);

    int radius = RadiusAndDirection.x;
    int2 dir = int2(1 - RadiusAndDirection.y, RadiusAndDirection.y);

    float4 accumulatedValue = float4(0.0, 0.0, 0.0, 0.0);

    for (int i = -radius; i <= radius; ++i)
    {
        uint cIndex = (uint) abs(i);
        accumulatedValue += Coefficients[cIndex >> 2][cIndex & 3] * t_Source[mad(i, dir, pixel)];
    }

    t_Destination[pixel] = accumulatedValue;
}
