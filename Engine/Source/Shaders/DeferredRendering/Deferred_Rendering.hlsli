
cbuffer cb_PerObject : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
};

cbuffer cb_PerFrame : register(b1)
{
    float3 cameraPosition;
    float deltaMs;
    float time;
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
    float4 position : SV_POSITION;
    float3 fragPos : FRAG_POS;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
};

struct PS_INPUT_GEOMPASS
{
    float4 sv_position : SV_POSITION;
    float3 fragPos : FRAG_POS;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
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
    float3 fragPos : FRAGPOS;
    float2 texCoords : TEXCOORD;
};

struct PS_INPUT_LIGHTPASS
{
    float4 sv_position : SV_POSITION;
    float3 fragPos : FRAGPOS;
    float2 texCoords : TEXCOORD;
};

