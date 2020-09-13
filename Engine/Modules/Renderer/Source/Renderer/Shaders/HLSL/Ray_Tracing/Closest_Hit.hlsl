#include "RayTrace_Common.hlsli"

struct Vertex
{
    float3 Position;
};

struct ShadowHitInfo
{
    bool isHit;
};

cbuffer LightBuffer : register(b1)
{
    float4 LightDir;
    float ShadowDarkness;
    float3 padding;
}

StructuredBuffer<Vertex> t_Vertex           : register(t0);
StructuredBuffer<int> indices               : register(t1);
RaytracingAccelerationStructure SceneBVH    : register(t2);

[shader("closesthit")]
void ClosestHit(inout HitInfo HitInfo, Attributes Attrib)
{

    //float3 lightDir = float3(0.2, 2, 6);
    float3 lightDir = (-LightDir.xyz);

    // Find the world - space hit position
    float3 worldOrigin = WorldRayOrigin() + RayTCurrent() * WorldRayDirection();

    //float3 lightDir = normalize(lightPos - worldOrigin);

    // Fire a shadow ray. The direction is hard-coded here, but can be fetched
    // from a constant-buffer
    RayDesc ray;
    ray.Origin = worldOrigin;
    ray.Direction = lightDir;
    ray.TMin = 0.01;
    ray.TMax = 100000;
    bool hit = true;

    // Initialize the ray payload
    ShadowHitInfo shadowPayload;
    shadowPayload.isHit = false;

    // Trace the shadow ray
    TraceRay(SceneBVH, RAY_FLAG_NONE, 0xFF, 1, 0, 1, ray, shadowPayload);

    float factor = shadowPayload.isHit ? ShadowDarkness : 1.0;
    float3 barycentrics = float3(1.f - Attrib.Barycentrics.x - Attrib.Barycentrics.y, Attrib.Barycentrics.x, Attrib.Barycentrics.y);
    float4 hitColor = float4(float3(factor, factor, factor), RayTCurrent());
    
    HitInfo.ColorAndDistance = float4(hitColor);
}