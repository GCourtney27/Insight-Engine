#include <../Common/Lights_Common.hlsli>

Texture2D t_DebugTexture : register(t0);

sampler s_LinearWrapampler : register(s0);

cbuffer cbLights : register(b0)
{
    PointLight pointLights[MAX_POINT_LIGHTS_SUPPORTED];
    DirectionalLight dirLight;
    SpotLight spotLights[MAX_SPOT_LIGHTS_SUPPORTED];
};
cbuffer cbPerFrame : register(b1)
{
    float3 cbCameraPosition;
    float cbCameraExposure; //4
    float4x4 cbView;
    float4x4 cbInverseView;
    float4x4 cbProjection;
    float4x4 cbInverseProjection;
    float cbCameraNearZ;
    float cbCameraFarZ;
    float cbDeltaMs;
    float cbTime; //4
    float cbNumPointLights;
    float cbNumDirectionalLights;
    float cbNumSpotLights;
    float cbRayTraceEnabled; //4
    float2 cbScreenSize;
    float padding1;
    float padding2;
};
void LinearizeDepth(inout float depth, float NearZ, float FarZ)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    depth = (2.0 * NearZ * FarZ) / (FarZ + NearZ - z * (FarZ - NearZ)) / FarZ;
}

struct PS_INPUT
{
    float4 sv_position : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

struct PS_OUT
{
    float4 FragColor : SV_Target;
};

PS_OUT main(PS_INPUT ps_in)
{
    PS_OUT ps_out;
    
    float depth = t_DebugTexture.Sample(s_LinearWrapampler, ps_in.texCoords).r;
    //LinearizeDepth(depth, cbCameraNearZ, cbCameraFarZ);
    //LinearizeDepth(depth, dirLight.NearZ, dirLight.FarZ);
    ps_out.FragColor = float4(depth, depth, depth, 1.0);
    
    return ps_out;
}
