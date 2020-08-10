#include "RayTrace_Common.hlsli"

struct Vertex
{
    float3 Position;
};

StructuredBuffer<Vertex> t_Vertex : register(t0);
StructuredBuffer<int> indices : register(t1);

[shader("closesthit")]
void ClosestHit(inout HitInfo HitInfo, Attributes Attrib)
{
    float3 barycentrics = float3(1.f - Attrib.Barycentrics.x - Attrib.Barycentrics.y, Attrib.Barycentrics.x, Attrib.Barycentrics.y);

    // '3 *' because Vertex struct only has a float 3 in it. So, 3 float offset will get to the next vertex
    uint VertexId = 3 * PrimitiveIndex();
    float3 HitColor = t_Vertex[indices[VertexId + 0]].Position * barycentrics.x +
                    t_Vertex[indices[VertexId + 1]].Position * barycentrics.y +
                    t_Vertex[indices[VertexId + 2]].Position * barycentrics.z;
    
    HitInfo.ColorAndDistance = float4(HitColor, RayTCurrent());
}