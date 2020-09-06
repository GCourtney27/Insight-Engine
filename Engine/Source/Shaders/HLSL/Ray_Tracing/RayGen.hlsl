#include "RayTrace_Common.dxr.hlsli"

// Raytracing output texture, accessed as a UAV
RWTexture2D<float4> gOutput : register(u0);

// Raytracing acceleration structure, accessed as a SRV
RaytracingAccelerationStructure SceneBVH : register(t0);

cbuffer CameraParams : register(b0)
{
    float4x4 view;
    float4x4 projection;
    float4x4 inverseView;
    float4x4 inverseProjection;
}

[shader("raygeneration")]
void RayGen()
{
    // Initialize the ray payload
    HitInfo HitInfo;
    HitInfo.ColorAndDistance = float4(1, 1, 1, 0);

    // Get the location within the dispatched 2D grid of work items
    // (often maps to pixels, so this could represent a pixel coordinate).
    uint2 PixelLaunchIndex = DispatchRaysIndex().xy;
    float2 Dimensions = float2(DispatchRaysDimensions().xy);
    float2 d = (((PixelLaunchIndex.xy + 0.5f) / Dimensions.xy) * 2.f - 1.f);
    float aspectRatio = Dimensions.x / Dimensions.y;

    // Define a ray, consisting of origin, direction, and the min-max distance values
    float4 target = mul(inverseProjection, float4(d.x, -d.y, 1, 1));
    RayDesc Ray;
    Ray.Origin = mul(inverseView, float4(0, 0, 0, 1));
    Ray.Direction = mul(inverseView, float4(target.xyz, 0));
    Ray.TMin = 0;
    Ray.TMax = 100000;
    
    // Trace the ray
    TraceRay(SceneBVH, RAY_FLAG_NONE, 0xFF, 0, 0, 0, Ray, HitInfo);

    gOutput[PixelLaunchIndex] = float4(HitInfo.ColorAndDistance.rgb, 1.f);
}
