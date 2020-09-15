//#include <../Common/Insight_Common.hlsli>
#include "RayTrace_Common.hlsli"
#pragma pack_matrix(row_major)

// Raytracing output texture, accessed as a UAV
RWTexture2D<float4> gOutput : register(u0);

// Raytracing acceleration structure, accessed as a SRV
RaytracingAccelerationStructure SceneBVH : register(t0);

cbuffer CameraParams : register(b0)
{
    float4x4 InverseView;
    float4x4 InverseProjection;
}

[shader("raygeneration")]
void RayGen()
{
    // Initialize the ray payload
    HitInfo HitInfo;
    HitInfo.ColorAndDistance = float4(1.0f, 1.0f, 1.0f, 0.0f);

    // Get the location within the dispatched 2D grid of work items
    // (often maps to pixels, so this could represent a pixel coordinate).
    uint2 PixelLaunchIndex = DispatchRaysIndex().xy;
    float2 Dimensions = float2(DispatchRaysDimensions().xy);
    float2 d = (((PixelLaunchIndex.xy + 0.5f) / Dimensions.xy) * 2.0f - 1.0f);
    
    // Define a ray, consisting of origin, direction, and the min-max distance values
    float4 Target = mul(float4(d.x, -d.y, 1, 1), InverseProjection);
    RayDesc Ray;
    Ray.Origin = mul(float4(0, 0, 0, 1), InverseView);
    Ray.Direction = mul(float4(Target.xyz, 0), InverseView);
    Ray.TMin = 0;
    Ray.TMax = 100000;
    
    // Trace the ray
    TraceRay(SceneBVH, RAY_FLAG_NONE, 0xFF, 0, 0, 0, Ray, HitInfo);

    gOutput[PixelLaunchIndex] = float4(HitInfo.ColorAndDistance.rgb, 1.0f);
}
