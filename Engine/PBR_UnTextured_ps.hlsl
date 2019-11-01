#include <PBR_Header.hlsli>

cbuffer dynamicLightBuffer : register(b0)
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

Texture2D normalSRV : NORMAL: register(t0);

TextureCube irradianceMapSRV : IRRADIANCE: register(t5);
TextureCube prefilterMapSRV : PREFILTERMAP: register(t6);
Texture2D brdfLUT : BRDF: register(t7);

SamplerState samplerState : SAMPLER: register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	// Sample Textures
	float3 albedoSample = color;

	float3 normalSample = normalSRV.Sample(samplerState, input.inTexCoord).xyz;

	float metallicSample = saturate(metallic);

   float roughnessSample = saturate(roughness);

   float aoSample = 0.3f;

   const float3x3 tanToView =
	   float3x3(normalize(input.inTangent),
		   normalize(input.inBiTangent),
		   normalize(input.inNormal));
   float3 N;
   N.x = normalSample.x * 2.0f - 1.0f;
   N.y = -normalSample.y * 2.0f + 1.0f;
   N.z = normalSample.z;
   N = mul(N, tanToView);

   float3 V = normalize(camPosition - input.inWorldPos); // View vector

   float3 F0 = float3(0.04, 0.04, 0.04);
   float3 baseReflectivity = lerp(F0, albedoSample, metallicSample);

   float3 Lo = float3(0.0f, 0.0f, 0.0f); // FInal lumanance of light

   float3 L = normalize(dynamicLightPosition - input.inWorldPos); // Light direction vector
   float3 H = normalize(V + L); // Halfway vector

   // -- Per light radiance -- //
   // Color ambient light (Not used)
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

   // -- IBL -- //
   // Irradiance map
   float3 F_IBL = fresnelSchlickRoughness(NdotV, baseReflectivity, roughnessSample);
   float3 kD_IBL = (1.0f - F_IBL) * (1.0f - metallicSample);
   float3 diffuse = irradianceMapSRV.Sample(samplerState, N).rgb * albedoSample * kD_IBL;
   //diffuse *= ambientLightStrength;

   // Specular IBL
   const float MAX_REFLECTION_LOD = 4.0f;
   float3 prefilteredColor = prefilterMapSRV.SampleLevel(samplerState, reflect(-V, N), roughnessSample * MAX_REFLECTION_LOD).rgb;
   float2 brdf = brdfLUT.Sample(samplerState, float2(NdotV, roughnessSample)).rg;
   float3 specular_IBL = prefilteredColor * (F_IBL * brdf.r + brdf.g);

   //specular_IBL *= ambientLightStrength;

   // Works, but everything is shiny
   float3 ambient = (diffuse + specular_IBL);// *(ambientLightStrength);
   float3 color = (ambient + Lo);

   // HDR tonemapping
   color = color / (color + float3(1.5f, 1.5f, 1.5f));
   // Gamma correct
   color = pow(color, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));

   return float4(color, 1.0f);
}