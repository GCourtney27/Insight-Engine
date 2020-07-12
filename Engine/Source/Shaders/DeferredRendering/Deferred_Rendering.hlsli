#define MAX_PER_OBJECT_LOD 9
#include <../Common/Lights_Common.hlsli>

cbuffer cbPerObject : register(b0)
{
    float4x4 world;
};

cbuffer cbPerFrame : register(b1)
{
    float3 cameraPosition;
    float cameraExposure;//4
    float4x4 view;
    float4x4 projection;
    float cameraNearZ;
    float cameraFarZ;
    float deltaMs;
    float time;//4
    float numPointLights;
    float numDirectionalLights;
    float numSpotLights;
    float padding;//4
    float2 screenSize;
	float padding1;
	float padding2;
    
    // TEMP
    float4x4 lightSpaceView;
    float4x4 lightSpaceProj;

};

cbuffer cbLights : register(b2)
{
    PointLight pointLights[MAX_POINT_LIGHTS_SUPPORTED];
    DirectionalLight dirLights[MAX_DIRECTIONAL_LIGHTS_SUPPORTED];
    SpotLight spotLights[MAX_SPOT_LIGHTS_SUPPORTED];
};

cbuffer cbPostFx : register(b3)
{
    // Vignette
    float vnInnerRadius;
    float vnOuterRadius;
    float vnOpacity;
    int vnEnabled;
    
	// Film Grain
    float fgStrength;
    int fgEnabled;
    
    // Chromatic Aberration
    int caEnabled;
    float caIntensity;
}

cbuffer PerObjectAdditive : register(b4)
{
    float roughnessAdditive;
    float metallicAdditive;
    float2 uvOffset;
    
    float2 tiling;
    float padding3;
    float padding4;
    
    float3 diffuseAdditive;
    float padding5;
};

/* Shadow Pass */
struct VS_INPUT_SHADOWPASS
{
    float3 position : POSITION;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
};

struct VS_OUTPUT_SHADOWPASS
{
    float4 sv_position : SV_POSITION;
    float3 fragPos : FRAG_POS;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
};

struct PS_INPUT_SHADOWPASS
{
    float4 sv_position : SV_POSITION;
    float3 fragPos : FRAG_POS;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
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
    float3 fragPosLightSpace : FRAG_POS_LIGHT_SPACE;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
};

struct PS_INPUT_GEOMPASS
{
    float4 sv_position : SV_POSITION;
    float3 fragPos : FRAG_POS;
    float3 fragPosLightSpace : FRAG_POS_LIGHT_SPACE;
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

