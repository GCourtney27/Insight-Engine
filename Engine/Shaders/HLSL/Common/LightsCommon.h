/*
    This file includes all lights that are able to be rendered inside a pixel shader
    * Note: Constant Buffers must be 16 byte aligned, edit with caution
*/
//#include "Shaders/HLSL/Common/InsightCommon.h"

// If max supported lights is changed here it must also be changed inside <Insight/Core.h>
#define MAX_POINT_LIGHTS_SUPPORTED 16
#define MAX_DIRECTIONAL_LIGHTS_SUPPORTED 1
#define MAX_SPOT_LIGHTS_SUPPORTED 16

//#include <PBRHelper.hlsli>

struct PointLight
{
    float3 position;
    float strength;
	
    float3 diffuse;
    float padding;
};

struct DirectionalLight
{
    float3 direction;
    float shadowDarknessMultiplier;
    
    float3 diffuse;
    float strength;
    
    float4x4 lightSpaceView;
    float4x4 lightSpaceProj;
    
    float NearZ;
    float FarZ;
    float2 Padding;
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


static const float PI = 3.14159265359;

float DistributionGGX(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float demon = NdotH * NdotH * (a2 - 1.0f) + 1.0f;
    demon = PI * demon * demon;
    return a2 / max(demon, 0.0000001); // Prevent divide by 0
}
float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float r = roughness + 1.0f;
    float k = (r * r) / 8.0f;
    float ggx1 = NdotV / (NdotV * (1.0f - k) + k); // Schlick GGX
    float ggx2 = NdotL / (NdotL * (1.0f - k) + k);
    return ggx1 * ggx2;
}
float3 FresnelSchlick(float HdotV, float3 baseReflectivity)
{
    // Base reflectivity in range from 0 to 1
    // returns range of base reflectivity to 1
    // inclreases as HdotV decreases (more reflectiviy when surface viewed at larger angles)
    return baseReflectivity + (1.0f - baseReflectivity) * pow(1.0f - HdotV, 5.0f);
}
float3 FresnelSchlickRoughness(float HdotV, float3 F0, float roughness)
{
    // Base reflectivity in range from 0 to 1
    // returns range of base reflectivity to 1
    // inclreases as HdotV decreases (more reflectiviy when surface viewed at larger angles)
    return F0 + (max(float3(1.0f - roughness, 1.0f - roughness, 1.0f - roughness), F0) - F0) * pow(1.0f - HdotV, 5.0f);
}


float3 CaclualteDirectionalLight(DirectionalLight Light, float3 ViewDirection, float3 WorldNormal, float3 WorldPosition, float NdotV, float3 MaterialAlbedo, float MaterialRoughness, float MaterialMetallic, float3 BaseReflectivity)
{
    float3 LightDir = normalize(Light.direction);
    float3 HalfwayDir = normalize(ViewDirection + LightDir);
    float3 Radiance = Light.diffuse * Light.strength;

    // Cook-Torrance BRDF
    float NdotL = max(dot(WorldNormal, LightDir), 0.0000001);
    float HdotV = max(dot(HalfwayDir, ViewDirection), 0.0);
    float NdotH = max(dot(WorldNormal, HalfwayDir), 0.0);

    float D = DistributionGGX(NdotH, MaterialRoughness);
    float G = GeometrySmith(NdotV, NdotL, MaterialRoughness);
    float3 F = FresnelSchlick(HdotV, BaseReflectivity);

    float3 Specular = D * G * F;
    Specular /= 4.0 * NdotV * NdotL;

    float3 kD = float3(1.0, 1.0, 1.0) - F;
    kD *= 1.0 - MaterialMetallic;

    return ((kD * MaterialAlbedo / PI + Specular) * Radiance * NdotL);
}

float3 CalculateSpotLight(SpotLight Light, float3 ViewDirection, float NdotV, float3 WorldPosition, float3 WorldNormal, float3 MaterialAlbedo, float MaterialRoughness, float MaterialMetallic, float3 BaseReflectivity)
{
    float3 LightDir = normalize(Light.position - WorldPosition);
    float3 HalfwayDir = normalize(LightDir + ViewDirection);
    float Distance = length(Light.position - WorldPosition);
    float Attenuation = 1.0 / (Distance * Distance);

    float Theta = dot(LightDir, normalize(-Light.direction));
    float Epsilon = Light.innerCutOff - Light.outerCutOff;
    float Intensity = clamp((Theta - Light.outerCutOff) / Epsilon, 0.0, 1.0);

    float3 radiance = ((Light.diffuse * (Light.strength * 10000.0)) * Intensity) * Attenuation;

    // Cook-Torrance BRDF
    float NdotL = max(dot(WorldNormal, LightDir), 0.0000001);
    float HdotV = max(dot(HalfwayDir, ViewDirection), 0.0);
    float NdotH = max(dot(WorldNormal, HalfwayDir), 0.0);

    float D = DistributionGGX(NdotH, MaterialRoughness);
    float G = GeometrySmith(NdotV, NdotL, MaterialRoughness);
    float3 F = FresnelSchlick(HdotV, BaseReflectivity);

    float3 Specular = D * G * F;
    Specular /= 4.0 * NdotV * NdotL;

    float3 kD = float3(1.0, 1.0, 1.0) - F;
    kD *= 1.0 - MaterialMetallic;

    return (kD * MaterialAlbedo / PI + Specular) * radiance * NdotL;
}

float3 CalculatePointLight(PointLight Light, float3 WorldPosition, float3 ViewDirection, float NdotV, float3 WorldNormal, float3 MaterialAlbedo, float MaterialMetallic, float MaterialRoughness, float3 BaseReflectivity)
{
    float3 LightDir = normalize(Light.position - WorldPosition);
    float3 HalfwayDir = normalize(ViewDirection + LightDir);
    float Distance = length(Light.position - WorldPosition);
    float Attenuation = 1.0 / (Distance * Distance);
    float3 Radiance = ((Light.diffuse * 255.0) * Light.strength) * Attenuation;

    // Cook-Torrance BRDF
    float NdotL = max(dot(WorldNormal, LightDir), 0.0000001);
    float HdotV = max(dot(HalfwayDir, ViewDirection), 0.0);
    float NdotH = max(dot(WorldNormal, HalfwayDir), 0.0);

    float D = DistributionGGX(NdotH, MaterialRoughness);
    float G = GeometrySmith(NdotV, NdotL, MaterialRoughness);
    float3 F = FresnelSchlick(HdotV, BaseReflectivity);

    float3 Specular = D * G * F;
    Specular /= 4.0 * NdotV * NdotL;

    float3 kD = float3(1.0, 1.0, 1.0) - F;
    kD *= 1.0 - MaterialMetallic;

    return (kD * MaterialAlbedo / PI + Specular) * Radiance * NdotL;
}
