
/*
    This file includes all lights that are able to be rendered inside a pixel shader
    *Note: Constant Buffers must be 4 byte aligned, edit with caution
*/

// If max supported lights is changed here it must also be changed inside the rendering context
#define MAX_POINT_LIGHTS_SUPPORTED 16
#define MAX_DIRECTIONAL_LIGHTS_SUPPORTED 4
#define MAX_SPOT_LIGHTS_SUPPORTED 16

struct PointLight
{
    float3 position;
    float strength;
	
    float3 diffuse;
    float padding1;
};

struct DirectionalLight
{
    float3 direction;
    float padding;
    
    float3 diffuse;
    float strength;
    
    float4x4 lightSpaceView;
    float4x4 lightSpaceProj;
};

struct SpotLight
{
    float3 position;
    float innerCutOff;
    float3 direction;
    float outerCutOff;

    float3 diffuse;
    float strength;
};
