#include <../Common/Lights_Common.hlsli>

Texture2D t_DebugTexture : register(t0);

sampler s_PointClampSampler : register(s0);

cbuffer cbLights : register(b0)
{
    PointLight pointLights[MAX_POINT_LIGHTS_SUPPORTED];
    DirectionalLight dirLight;
    SpotLight spotLights[MAX_SPOT_LIGHTS_SUPPORTED];
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
    
    float3 depth = t_DebugTexture.Sample(s_PointClampSampler, ps_in.texCoords);
    //LinearizeDepth(depth, dirLight.NearZ, dirLight.FarZ);
    ps_out.FragColor = float4(depth, 1.0);
    
    return ps_out;
}
