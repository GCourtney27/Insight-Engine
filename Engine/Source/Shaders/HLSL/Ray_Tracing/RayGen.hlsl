#include "Common.hlsl"

// Raytracing output texture, accessed as a UAV
RWTexture2D<float4> gOutput : register(u0);

// Raytracing acceleration structure, accessed as a SRV
RaytracingAccelerationStructure SceneBVH : register(t0);

cbuffer cbPerFrame : register(b0)
{
    float3 cameraPosition;
    float cameraExposure; //4
    float4x4 view;
    float4x4 inverseView;
    float4x4 projection;
    float4x4 inverseProjection;
    float cameraNearZ;
    float cameraFarZ;
    float DeltaMs;
    float time; //4
    float numPointLights;
    float numDirectionalLights;
    float numSpotLights;
    float padding; //4
    float2 screenSize;
    float padding1;
    float padding2;
};

[shader("raygeneration")]
void RayGen()
{
    // Initialize the ray payload
    HitInfo payload;
    payload.colorAndDistance = float4(0, 0, 0, 0);

    // Get the location within the dispatched 2D grid of work items
    // (often maps to pixels, so this could represent a pixel coordinate).
    uint2 launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 d = (((launchIndex.xy + 0.5f) / dims.xy) * 2.f - 1.f);

    float aspectRatio = dims.x / dims.y;
  // Perspective
    RayDesc ray;
    // Define a ray, consisting of origin, direction, and the min-max distance values
    //ray.Origin = mul(inverseView, float4(0, 0, 0, 1));
    //float4 target = mul(inverseProjection, float4(d.x, -d.y, 1, 1));
    //ray.Direction = mul(inverseView, float4(target.xyz, 0));
    ray.Origin = float3(d.x, -d.y, 1);
    ray.Direction = float3(0, 0, -1);
    ray.TMin = 0;
    ray.TMax = 100000;
    
  // Trace the ray
    TraceRay(SceneBVH, RAY_FLAG_NONE, 0xFF, 0, 0, 0, ray, payload);

    gOutput[launchIndex] = float4(payload.colorAndDistance.rgb, 1.f);
}
