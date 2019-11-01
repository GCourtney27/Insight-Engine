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
SamplerState samplerState : SAMPLER: register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    // Sample Textures
    float3 albedoSample = saturate(albedoSRV.Sample(samplerState, input.inTexCoord).rgb + color);
    float3 normalSample = saturate(normalSRV.Sample(samplerState, input.inTexCoord).xyz);
    float metallicSample = saturate(metallicSRV.Sample(samplerState, input.inTexCoord).r + metallic);
	float roughnessSample = saturate(roughnessSRV.Sample(samplerState, input.inTexCoord).r + roughness);
	float aoSample = aoSRV.Sample(samplerState, input.inTexCoord).r;
    
    // Transform Normals From Tangent Space to View Space
    const float3x3 tanToView =  float3x3(normalize(input.inTangent),
                                         normalize(input.inBiTangent),
                                         normalize(input.inNormal));
    float3 N;
            N.x = normalSample.x * 2.0f - 1.0f;
            N.y = -normalSample.y * 2.0f + 1.0f;
            N.z = normalSample.z;
            N = mul(N, tanToView);

    // View vector
    float3 V = normalize(camPosition - input.inWorldPos); 

	float3 F0 = float3(0.04, 0.04, 0.04);
    float3 baseReflectivity = lerp(F0, albedoSample, metallicSample);

    float3 Lo = float3(0.0f, 0.0f, 0.0f); // Final lumanance of light

    //for (int i = 0; i < 2; i++)
    //{

    //}

    float3 L = normalize(dynamicLightPosition - input.inWorldPos); // Light direction vector
    //float3 L = normalize(directionalLightDirection - input.inWorldPos); // Light direction vector
    float3 H = normalize(V + L); // Halfway vector

    // -- Per light radiance -- //
    // Color ambient light (Not used)
    // Disable this for directional light
    float3 ambientLight = ambientLightColor * ambientLightStrength;
    float3 appliedLight = ambientLight;

    float3 vectorToLight = normalize(directionalLightDirection - input.inWorldPos);
    float3 diffuseLightIntensity = max(dot(L, N), 0);
    float distanceToLight = distance(directionalLightDirection, input.inWorldPos);
    // Disable this for directional light
    float attenuationFactor = 1 / (dynamicLightAttenuation_a + dynamicLightAttenuation_b * distanceToLight + dynamicLightAttenuation_c * pow(distanceToLight, 2));
    // Disable this for directional light
    diffuseLightIntensity *= attenuationFactor;
    // Disable this for directional light
    float3 diffuseLight = diffuseLightIntensity * dynamicLightStrength * dynamicLightColor;
    //float3 diffuseLight = diffuseLightIntensity * directionalLightStrength * directionalLightColor;
    // Disable this for directional light
    appliedLight += diffuseLight;
    float3 radiance = albedoSample * diffuseLight; // Works
  
    // Cook-Torrance BRDF
    float NdotV = max(dot(N, V), 0.0000001);
    float NdotL = max(dot(N, L), 0.0000001);
    float HdotV = max(dot(H, V), 0.0f);
    float NdotH = max(dot(N, H), 0.0f);

    float D = distributionGGX(NdotH, roughnessSample);
    float G = geometrySmith(NdotV, NdotL, roughnessSample);
    float3 F = fresnelSchlick(HdotV, baseReflectivity);

    float3 specular = D * G * F; 
    specular /= 4.0f * NdotV * NdotL; 

    float3 kD = float3(1.0f, 1.0f, 1.0f) - F;
    
    kD *= 1.0f - metallicSample;

    Lo += (kD * albedoSample / PI + specular) * radiance * NdotL;
    

    // -- IBL -- //
    // Irradiance map
    float3 F_IBL = fresnelSchlickRoughness(NdotV, baseReflectivity, roughnessSample);
	float3 kD_IBL = (1.0f - F_IBL) * (1.0f - metallicSample);
	float3 diffuse = irradianceMapSRV.Sample(samplerState, N).rgb * albedoSample * kD_IBL;

    // Specular IBL
    const float MAX_REFLECTION_LOD = 4.0f;
    float3 environmentMapColor = environmentMapSRV.SampleLevel(samplerState, reflect(-V, N), roughnessSample * MAX_REFLECTION_LOD).rgb;
    float2 brdf = brdfLUT.Sample(samplerState, float2(NdotV, roughnessSample)).rg;
    float3 specular_IBL = environmentMapColor * (F_IBL * brdf.r + brdf.g);

    // Add lighting to IBL velues for final color
	float3 ambient = (diffuse + specular_IBL) * (0.5f);
    float3 color = (ambient + Lo);
    
    // HDR tonemapping
    color = color / (color + float3(1.5f, 1.5f, 1.5f));
    // Gamma correction
    color = pow(color, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));

    return float4(color, 1.0f);
}