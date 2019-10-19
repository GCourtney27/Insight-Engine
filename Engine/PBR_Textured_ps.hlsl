
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

struct PS_INPUT
{
    float4 inPosition : SV_POSITION; // Screen space pixel position
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBiTangent : BITANGENT;
    float3 inWorldPos : WORLD_POSITION; // World space pixel position
};

Texture2D albedoSRV : TEXTURE : register(t0);
Texture2D normalSRV : NORMAL : register(t1);
Texture2D metallicSRV : METALLIC : register(t2);
Texture2D roughnessSRV : ROUGHNESS : register(t3);

TextureCube irradianceMapSRV : IRRADIANCE : register(t4);
TextureCube prefilterMapSRV : PREFILTERMAP : register(t5);
Texture2D brdfLUT : BRDF : register(t6);

SamplerState samplerState : SAMPLER: register(s0);

static const float PI = 3.14159265359;

float distributionGGX(float NdotH, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float demon = NdotH * NdotH * (a2 - 1.0f) + 1.0f;
	demon = PI * demon * demon;
	return a2 / max(demon, 0.0000001); // Prevent divide by 0
}
float geometrySmith(float NdotV, float NdotL, float roughness)
{
	float r = roughness + 1.0f;
	float k = (r * r) / 8.0f;
	float ggx1 = NdotV / (NdotV * (1.0f - k) + k); // Schlick GGX
	float ggx2 = NdotL / (NdotL * (1.0f - k) + k);
	return ggx1 * ggx2;
}
float3 fresnelSchlick(float HdotV, float3 baseReflectivity)
{
	// Base reflectivity in range from 0 to 1
	// returns range of base reflectivity to 1
	// inclreases as HdotV decreases (more reflectiviy when surface viewed at larger angles)
	return baseReflectivity + (1.0f - baseReflectivity) * pow(1.0f - HdotV, 5.0f);
}
float3 fresnelSchlickRoughness(float HdotV, float3 F0, float roughness)
{
	// Base reflectivity in range from 0 to 1
	// returns range of base reflectivity to 1
	// inclreases as HdotV decreases (more reflectiviy when surface viewed at larger angles)
    return F0 + (max(float3(1.0f - roughness, 1.0f - roughness, 1.0f - roughness), F0) - F0) * pow(1.0f - HdotV, 5.0f);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    // Sample Textures
    float3 albedoSample = albedoSRV.Sample(samplerState, input.inTexCoord).rgb;
    float3 normalSample = normalSRV.Sample(samplerState, input.inTexCoord).xyz;
    float metallicSample = metallicSRV.Sample(samplerState, input.inTexCoord).r;
    float roughnessSample = roughnessSRV.Sample(samplerState, input.inTexCoord).r;
    //float3 albedoSample = float3(1.0f, 0.2f, 0.2f);
    //float metallicSample = 0.2f;
    //float roughnessSample = 0.4f;

    // Transform Normals From Tangent Space to View Space
    const float3x3 tanToView = 
    float3x3(normalize(input.inTangent),
             normalize(input.inBiTangent),
             normalize(input.inNormal));
    float3 N;
    N.x = normalSample.x * 2.0f - 1.0f;
    N.y = -normalSample.y * 2.0f + 1.0f;
    N.z = normalSample.z;
    N = mul(N, tanToView);

    float3 V = normalize(camPosition - input.inWorldPos);

    float3 baseReflectivity = lerp(float3(0.04, 0.04, 0.04), albedoSample, metallicSample);

    float3 Lo = float3(0.0f, 0.0f, 0.0f);

    float3 L = normalize(dynamicLightPosition - input.inWorldPos); // Light vector
    float3 H = normalize(V + L); //Halfway vector


    float3 ambientLight = ambientLightColor * ambientLightStrength;
    float3 appliedLight = ambientLight;

    float3 vectorToLight = normalize(dynamicLightPosition - input.inWorldPos);
    float3 diffuseLightIntensity = max(dot(L, N), 0);
    float distanceToLight = distance(dynamicLightPosition, input.inWorldPos);
    float attenuationFactor = 1 / (dynamicLightAttenuation_a + dynamicLightAttenuation_b * distanceToLight + dynamicLightAttenuation_c * pow(distanceToLight, 2));
    diffuseLightIntensity *= attenuationFactor;
    float3 diffuseLight = diffuseLightIntensity * dynamicLightStrength * dynamicLightColor;
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

    float3 specular = D * G * F; // Works
    specular /= 4.0f * NdotV * NdotL; // Works

    float3 kD = float3(1.0f, 1.0f, 1.0f) - F; // Works
    
    kD *= 1.0f - metallicSample; // Works

    Lo += (kD * albedoSample / PI + specular) * radiance * NdotL;

    // IBL
    //float3 F_IBL = fresnelSchlick(NdotV, baseReflectivity);
    //float3 kD_IBL = (1.0f - F_IBL) * (1.0f - metallicSample);
    //float3 irradianceMapSample = irradianceMapSRV.Sample(samplerState, input.inNormal).rgb * albedoSample * kD_IBL;
    //float3 ambient = irradianceMapSample;

    float3 F_IBL = fresnelSchlickRoughness(NdotV, baseReflectivity, roughnessSample);
    float3 kD_IBL = (1.0f - F_IBL) * (1.0f - metallicSample);
    float3 diffuse = irradianceMapSRV.Sample(samplerState, N).rgb * albedoSample * kD_IBL;

    const float MAX_REFLECTION_LOD = 4.0f;
    float3 prefilteredColor = prefilterMapSRV.SampleLevel(samplerState, reflect(-V, N), roughnessSample * MAX_REFLECTION_LOD);
    float2 brdf = brdfLUT.Sample(samplerState, float2(NdotV, roughnessSample)).rg;
    float3 specular_IBL = prefilteredColor * (F_IBL * brdf.r + brdf.g);

    float3 ambient = (diffuse + specular_IBL);

    float3 color = ambient + Lo;
    //float3 color = Lo; // Works

    // HDR tonemapping;
    color = color / (color + float3(1.5f, 1.5f, 1.5f));
    // Gamma correct
    color = pow(color, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));

    return float4(color, 1.0f);
}