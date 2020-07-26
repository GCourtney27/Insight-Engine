#include <Deferred_Rendering.hlsli>	
#include <../Common/PBR_Helper.hlsli>
#define SHADOW_DEPTH_BIAS 0.00005f

// Texture Inputs
// --------------
Texture2D t_AlbedoGBuffer               : register(t0);
Texture2D t_NormalGBuffer               : register(t1);
Texture2D t_RoughnessMetallicAOGBuffer  : register(t2);
Texture2D t_PositionGBuffer             : register(t3);
Texture2D t_SceneDepthGBuffer           : register(t4);

Texture2D t_ShadowDepth         : register(t10);

TextureCube tc_IrradianceMap    : register(t11);
TextureCube tc_EnvironmentMap   : register(t12);
Texture2D t_BrdfLUT             : register(t13);

// Samplers
// --------
sampler s_PointClampSampler : register(s0);
sampler s_LinearWrapSampler : register(s1);

// Function Signatures
// -------------------
void GammaCorrect(inout float3 target);
void HDRToneMap(inout float3 target);
float ShadowCalculation(float4 fragPosLightSpace, float3 normal, float3 lightDir);

// Pixel Shader Return Value
// -------------------------
struct PS_OUTPUT_LIGHTPASS
{
    float3 litImage : SV_Target;
};

// Entry Point
// -----------
PS_OUTPUT_LIGHTPASS main(PS_INPUT_LIGHTPASS ps_in)
{
    PS_OUTPUT_LIGHTPASS ps_out;
    
	// Sample Textures
    float3 albedo = pow(abs(t_AlbedoGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb), float3(2.2, 2.2, 2.2));
    float3 roughMetAOBufferSample = t_RoughnessMetallicAOGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    float3 worldPosition = t_PositionGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).xyz;
    float3 normal = t_NormalGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).xyz;
    float sceneDepth = t_SceneDepthGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).r;
    float roughness = roughMetAOBufferSample.r;
    float metallic = roughMetAOBufferSample.g;
    float ambientOcclusion = roughMetAOBufferSample.b;
    
    float3 viewDirection = normalize(cameraPosition - worldPosition);
        
    float3 F0 = float3(0.04, 0.04, 0.04);
    float3 baseReflectivity = lerp(F0, albedo, metallic);
    float NdotV = max(dot(normal, viewDirection), 0.0000001);
    
    float3 spotLightLuminance = float3(0.0, 0.0, 0.0);
    float3 pointLightLuminance = float3(0.0, 0.0, 0.0);
    float3 directionalLightLuminance = float3(0.0, 0.0, 0.0);
    
    // Calculate Light Radiance
    // Directional Lights
    for (int d = 0; d < numDirectionalLights; d++)
    {
        float3 lightDir = normalize(-dirLights[d].direction);
        float3 halfwayDir = normalize(viewDirection + lightDir);
        float3 radiance = (dirLights[d].diffuse) * dirLights[d].strength;
        
        // Cook-Torrance BRDF
        float NdotL = max(dot(normal, lightDir), 0.0000001);
        float HdotV = max(dot(halfwayDir, viewDirection), 0.0);
        float NdotH = max(dot(normal, halfwayDir), 0.0);
        
        float D = DistributionGGX(NdotH, roughness);
        float G = GeometrySmith(NdotV, NdotL, roughness);
        float3 F = FresnelSchlick(HdotV, baseReflectivity);
        
        float3 specular = D * G * F;
        specular /= 4.0 * NdotV * NdotL;
        
        float3 kD = float3(1.0, 1.0, 1.0) - F;
        kD *= 1.0 - metallic;
        
        // Shadowing
        //float4 fragPosLightSpace = mul(float4(worldPosition, 1.0), mul(dirLights[d].lightSpaceView, dirLights[d].lightSpaceProj));
        float shadow = 0.0f; //ShadowCalculation(fragPosLightSpace, normal, lightDir);
        //ps_out.litImage = float3(shadow, shadow, shadow);
        //return ps_out;
        
        directionalLightLuminance += ((kD * albedo / PI + specular) * radiance * NdotL);// * (1.0 - shadow);

    }
    
    // Spot Lights
    for (int s = 0; s < numPointLights; s++)
    {
        float3 lightDir = normalize(spotLights[s].position - worldPosition);
        float3 halfwayDir = normalize(lightDir + viewDirection);
        float distance = length(spotLights[s].position - worldPosition);
        float attenuation = 1.0 / (distance * distance);
        
        float theta = dot(lightDir, normalize(-spotLights[s].direction));
        float epsilon = spotLights[s].innerCutOff - spotLights[s].outerCutOff;
        float intensity = clamp((theta - spotLights[s].outerCutOff) / epsilon, 0.0, 1.0);

        float3 radiance = ((spotLights[s].diffuse * (spotLights[s].strength * 10000.0)) * intensity) * attenuation;
        
         // Cook-Torrance BRDF
        float NdotL = max(dot(normal, lightDir), 0.0000001);
        float HdotV = max(dot(halfwayDir, viewDirection), 0.0);
        float NdotH = max(dot(normal, halfwayDir), 0.0);
        
        float D = DistributionGGX(NdotH, roughness);
        float G = GeometrySmith(NdotV, NdotL, roughness);
        float3 F = FresnelSchlick(HdotV, baseReflectivity);
        
        float3 specular = D * G * F;
        specular /= 4.0 * NdotV * NdotL;
        
        float3 kD = float3(1.0, 1.0, 1.0) - F;
        kD *= 1.0 - metallic;
        
        spotLightLuminance += (kD * albedo / PI + specular) * radiance * NdotL;
    }
    
    // Point Lights
    for (int p = 0; p < numPointLights; p++)
    {
        float3 lightDir = normalize(pointLights[p].position - worldPosition);
        float3 halfwayDir = normalize(viewDirection + lightDir);
        float distance = length(pointLights[p].position - worldPosition);
        float attenuation = 1.0 / (distance * distance);
        float3 radiance = ((pointLights[p].diffuse * 255.0) * pointLights[p].strength) * attenuation;
        
        // Cook-Torrance BRDF
        float NdotL = max(dot(normal, lightDir), 0.0000001);
        float HdotV = max(dot(halfwayDir, viewDirection), 0.0);
        float NdotH = max(dot(normal, halfwayDir), 0.0);
        
        float D = DistributionGGX(NdotH, roughness);
        float G = GeometrySmith(NdotV, NdotL, roughness);
        float3 F = FresnelSchlick(HdotV, baseReflectivity);
        
        float3 specular = D * G * F;
        specular /= 4.0 * NdotV * NdotL;
        
        float3 kD = float3(1.0, 1.0, 1.0) - F;
        kD *= 1.0 - metallic;
        
        pointLightLuminance += (kD * albedo / PI + specular) * radiance * NdotL;
    }
    
    // IBL
    // Irradiance
    float3 F_IBL = FresnelSchlickRoughness(NdotV, baseReflectivity, roughness);
    float3 kD_IBL = (1.0f - F_IBL) * (1.0f - metallic);
    float3 diffuse_IBL = tc_IrradianceMap.Sample(s_LinearWrapSampler, normal).rgb * albedo * kD_IBL;

    // Specular IBL
    const float MAX_REFLECTION_MIP_LOD = 10.0f;
    float3 environmentMapColor = tc_EnvironmentMap.SampleLevel(s_LinearWrapSampler, reflect(-viewDirection, normal), roughness * MAX_REFLECTION_MIP_LOD).rgb;
    float2 brdf = t_BrdfLUT.Sample(s_LinearWrapSampler, float2(NdotV, roughness)).rg;
    float3 specular_IBL = environmentMapColor * (F_IBL * brdf.r + brdf.g);
    
    float3 ambient = (diffuse_IBL + specular_IBL) * ambientOcclusion;
    float3 combinedLightLuminance = (pointLightLuminance + spotLightLuminance) + (directionalLightLuminance);
    
     // Combine Light Luminance
    float3 pixelColor = ambient + combinedLightLuminance;
    
    HDRToneMap(pixelColor);
    GammaCorrect(pixelColor);
    
    ps_out.litImage.rgb = pixelColor;
    
    return ps_out;
}

void HDRToneMap(inout float3 target)
{
    target = float3(1.0, 1.0, 1.0) - exp(-target * cameraExposure);
}

void GammaCorrect(inout float3 target)
{
    const float gamma = 2.2;
    target = pow(abs(target.rgb), float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));
}

float ShadowCalculation(float4 fragPosLightSpace, float3 normal, float3 lightDir)
{
    float3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // Soften Shadows
    float shadow = 0.0;
    float2 texelSize = 1.0 / float2(1024.0, 1024.0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float depth = t_ShadowDepth.Sample(s_PointClampSampler, projCoords.xy + float2(x, y) * texelSize).r;
            shadow += (depth + bias) < projCoords.z ? 0.0 : 1.0;

        }
    }
    return (1.0 - shadow) / 9.0;
}
