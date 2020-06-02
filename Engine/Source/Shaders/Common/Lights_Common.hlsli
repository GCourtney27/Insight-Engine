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
};

struct SpotLight
{
    float3 position;
    float3 direction;
    float innerCutOff;
    float outerCutOff;

    float3 ambient;
    float3 diffuse;
    float3 specular;

    float constantFactor;
    float linearFactor;
    float quadraticFactor;
};
