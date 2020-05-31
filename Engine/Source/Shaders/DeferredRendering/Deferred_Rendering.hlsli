// TODO Make a light common include file that is accessable for standalone and debug builds
#include <../Common/Lights_Common.hlsli>

cbuffer cbPerObject : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
};

cbuffer cbPerFrame : register(b1)
{
    float3 cameraPosition;
    float cameraExposure;
    float cameraNearZ;
    float cameraFarZ;
    float deltaMs;
    float time;
    int numPointLights;
    int numDirectionalLights;
    int numSpotLights;
};

cbuffer cbLights : register(b2)
{
    PointLight pointLights;
    //DirectionalLight dirLight[MAX_DIRECTIONAL_LIGHTS_SUPPORTED];
    //SpotLight spotLight[MAX_SPOT_LIGHTS_SUPPORTED];
};

/* Geometry Pass */
struct VS_INPUT_GEOMPASS
{
    float3 position : POSITION;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
};

struct VS_OUTPUT_GEOMPASS
{
    float4 sv_position : SV_POSITION;
    float3 fragPos : FRAG_POS;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
};

struct PS_INPUT_GEOMPASS
{
    float4 sv_position : SV_POSITION;
    float3 fragPos : FRAG_POS;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
};

/* Lighting Pass */
struct VS_INPUT_LIGHTPASS
{
    float3 position : POSITION;
    float2 texCoords : TEXCOORD;
};

struct VS_OUTPUT_LIGHTPASS
{
    float4 sv_position : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

struct PS_INPUT_LIGHTPASS
{
    float4 sv_position : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

