#include <PBR_Header.hlsli>

cbuffer lightBuffer : register(b0)
{
	// Initialize in Graphics::InitializeScene and updated in Graphics::RenderFrame
    float3 ambientLightColor;
    float ambientLightStrength;

    float3 dynamicLightColor;
    float dynamicLightStrength;
    float3 dynamicLightPosition;

    float dynamicLightAttenuation_a;
    float dynamicLightAttenuation_b;
    float dynamicLightAttenuation_c;
}

cbuffer PerFrame : register(b1)
{
    float3 camPosition;
    float deltaTime;
}

cbuffer PerObjectColor : register(b2)
{
    float3 color;
    float metallic;
    float roughness;
}

cbuffer DirectionalLight : register(b3)
{
    float3 directionalLightColor;
    float directionalLightStrength;
    float3 directionalLightDirection;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION; // Screen space pixel position
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBiTangent : BITANGENT;
    float3 inWorldPos : WORLD_POSITION; // World space pixel position
};

// Base textured needed for a textured pipline
Texture2D albedoSRV : TEXTURE : register(t0);
Texture2D normalSRV : NORMAL : register(t1);
Texture2D metallicSRV : METALLIC : register(t2);
Texture2D roughnessSRV : ROUGHNESS : register(t3);
Texture2D aoSRV : ROUGHNESS : register(t4);

// Textures needed for Image Based Lighting
TextureCube irradianceMapSRV : IRRADIANCE : register(t5);
TextureCube environmentMapSRV : PREFILTERMAP : register(t6);
Texture2D brdfLUT : BRDF : register(t7);

// Sampler textures
SamplerState samplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    // Sample Textures
    //float3 albedoSample = saturate(albedoSRV.Sample(samplerState, input.inTexCoord).rgb + color);
    float3 albedoSample = pow(albedoSRV.Sample(samplerState, input.inTexCoord).rgb, color);
    float3 normalSample = (normalSRV.Sample(samplerState, input.inTexCoord).xyz);
    float metallicSample = (metallicSRV.Sample(samplerState, input.inTexCoord).r + metallic);
    float roughnessSample = (roughnessSRV.Sample(samplerState, input.inTexCoord).r + roughness);
	float aoSample = aoSRV.Sample(samplerState, input.inTexCoord).r;
    
    // Transform Normals From Tangent Space to View Space
    const float3x3 tanToView = float3x3(normalize(input.inTangent),
                                         normalize(input.inBiTangent),
                                         normalize(input.inNormal));
    float3 N;
    N.x = normalSample.x * 2.0f - 1.0f;
    N.y = -normalSample.y * 2.0f + 1.0f;
    N.z = normalSample.z;
    N = normalize(mul(N, tanToView));

    // View vector
    float3 V = normalize(camPosition - input.inWorldPos);

    float3 F0 = float3(0.04, 0.04, 0.04);
    float3 baseReflectivity = lerp(F0, albedoSample, metallicSample);

    float3 Lo = float3(0.0f, 0.0f, 0.0f); // Final lumanance of light

    float3 P_L = normalize(dynamicLightPosition - input.inWorldPos); // Light direction vector
    float3 Dir_L = normalize(directionalLightDirection - input.inWorldPos); // Light direction vector
    float3 P_H = normalize(V + P_L); // Halfway vector
    float3 Dir_H = normalize(V + Dir_L); // Halfway vector

    // -- Per light radiance -- //

    float3 vectorToLight = normalize(dynamicLightPosition - input.inWorldPos);
    float distanceToLight = distance(vectorToLight, input.inWorldPos);
    float3 diffuseLightIntensity = max(dot(P_L, N), 0);
    float attenuationFactor = 1 / (dynamicLightAttenuation_a + dynamicLightAttenuation_b * distanceToLight + dynamicLightAttenuation_c * pow(distanceToLight, 2));
    diffuseLightIntensity *= attenuationFactor;
    float3 diffuseLight = diffuseLightIntensity * dynamicLightStrength * dynamicLightColor;

    float3 pointLightRadiance = (albedoSample * diffuseLight);
    float3 directionalLight = saturate(dot(directionalLightDirection, N) * directionalLightColor);
    
    float3 directionalLightRadiance = (albedoSample * directionalLight) * directionalLightStrength;

    // Cook-Torrance BRDF
    float NdotV = max(dot(N, V), 0.0000001);
    
    float P_NdotL = max(dot(N, P_L), 0.0000001);
    float P_HdotV = max(dot(P_H, V), 0.0f);
    float P_NdotH = max(dot(N, P_H), 0.0f);
    
    float P_D = distributionGGX(P_NdotH, roughnessSample);
    float P_G = geometrySmith(NdotV, P_NdotL, roughnessSample);
    float3 P_F = fresnelSchlick(P_HdotV, baseReflectivity);
    
    float3 P_specular = P_D * P_G * P_F;
    P_specular /= 4.0f * NdotV * P_NdotL;

    float3 P_kD = float3(1.0f, 1.0f, 1.0f) - P_F;
    
    P_kD *= 1.0f - metallicSample;

    Lo += (P_kD * albedoSample / PI + P_specular) * pointLightRadiance * P_NdotL;
    
    float Dir_NdotH = max(dot(N, Dir_H), 0.0f);
    float Dir_HdotV = max(dot(Dir_H, V), 0.0f);
    float Dir_NdotL = max(dot(N, Dir_L), 0.0000001);

    float Dir_D = distributionGGX(Dir_NdotH, roughnessSample);
    float Dir_G = geometrySmith(NdotV, Dir_NdotL, roughnessSample);
    float3 Dir_F = fresnelSchlick(Dir_HdotV, baseReflectivity);

    float3 Dir_specular = Dir_D * Dir_G * Dir_F;
    Dir_specular /= 4.0f * NdotV * Dir_NdotL;

    float3 Dir_kD = float3(1.0f, 1.0f, 1.0f) - Dir_F;
    
    Dir_kD *= 1.0f - metallicSample;

    Lo += (Dir_kD * albedoSample / PI + Dir_specular) * directionalLightRadiance * Dir_NdotL;
    
    // -- IBL -- //
    // Irradiance map
    float3 F_IBL = fresnelSchlickRoughness(NdotV, baseReflectivity, roughnessSample);
    float3 kD_IBL = (1.0f - F_IBL) * (1.0f - metallicSample);
    float3 diffuse = irradianceMapSRV.Sample(samplerState, N).rgb * albedoSample * kD_IBL;

    // Specular IBL
    const float MAX_REFLECTION_LOD = 4.0f;
    float3 environmentMapColor = environmentMapSRV.SampleLevel(samplerState, reflect(-V, N), roughnessSample * MAX_REFLECTION_LOD).rgb;
    float2 brdf = brdfLUT.Sample(samplerState, float2(NdotV, roughnessSample)).rg;
    float3 specular_IBL = environmentMapColor * (F_IBL * brdf.r + brdf.g) ;
    
    float3 ambientLight = ambientLightColor * ambientLightStrength;
    float3 diffuse_IBL = (diffuse + specular_IBL) * ambientLight;

    // Add lighting to IBL velues for final color
    float3 ambient = diffuse_IBL * aoSample;
    //float3 ambient = (diffuse) * ambientLight;
    float3 color = (ambient + Lo);
    
    // HDR tonemapping
    color = color / (color + float3(1.0f, 1.0f, 1.0f));
    // Gamma correction
    color = pow(color, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));


    return float4(color, 1.0f);
}