#include "Common.hlsl"

struct STriVertex
{
    float3 position;
    float2 texCoord;
};

StructuredBuffer<STriVertex> BTriVertex : register(t0);
StructuredBuffer<int> indices : register(t1);

[shader("closesthit")]
void ClosestHit(inout HitInfo payload, Attributes attrib)
{
    float3 barycentrics =
  float3(1.f - attrib.bary.x - attrib.bary.y, attrib.bary.x, attrib.bary.y);

    uint vertId = 3 * PrimitiveIndex();
    float3 hitColor = BTriVertex[indices[vertId + 0]].position * barycentrics.x +
                    BTriVertex[indices[vertId + 1]].position * barycentrics.y +
                    BTriVertex[indices[vertId + 2]].position * barycentrics.z;

    payload.colorAndDistance = float4(hitColor, RayTCurrent());
}