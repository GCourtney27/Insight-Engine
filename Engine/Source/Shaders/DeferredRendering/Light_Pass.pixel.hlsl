#include <Deferred_Rendering.hlsli>	
#include <../Common/PBR_Helper.hlsli>

// Texture Inputs
// --------------
Texture2D t_AlbedoGBuffer : register(t0);
Texture2D t_NormalGBuffer : register(t1);
Texture2D t_RoughnessMetallicAOGBuffer : register(t2);
Texture2D t_PositionGBuffer : register(t3);
Texture2D t_DepthGBuffer : register(t4);

TextureCube tc_IrradianceMap : register(t10);
TextureCube tc_EnvironmentMap : register(t11);
Texture2D t_BrdfLUT : register(t12);

// Samplers
// --------
sampler s_LinearWrapSampler : register(s0);

// Function signatures
// -------------------
float3 CalculatePointLight(PointLight light, float3 normal, float3 fragPosition, float3 viewDirection, float2 texCoords);
void GammaCorrect(inout float3 target);
void HDRToneMap(inout float3 target);
float LinearizeDepth(float depth);

struct PS_OUTPUT_LIGHTPASS
{
    float4 litImage : SV_Target0;
};

// Entry Point
// -----------
PS_OUTPUT_LIGHTPASS main(PS_INPUT_LIGHTPASS ps_in) : SV_TARGET
{
    PS_OUTPUT_LIGHTPASS ps_out;
    
	// Sample Textures
    float3 albedoBufferSample = pow(t_AlbedoGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb, float3(2.2, 2.2, 2.2));
    float3 roughMetAOBufferSample = t_RoughnessMetallicAOGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    float3 positionBufferSample = t_PositionGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    float3 normalBufferSample = t_NormalGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    float depthBufferSample = t_DepthGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).r;
    float roughnessSample = roughMetAOBufferSample.r;
    float metallicSample = roughMetAOBufferSample.g;
    float aoSample = roughMetAOBufferSample.b;
    
    float3 normal = (normalBufferSample);
    float3 viewDirection = normalize(cameraPosition - positionBufferSample);
    
    float3 F0 = float3(0.04, 0.04, 0.04);
    float3 baseReflectivity = lerp(F0, albedoBufferSample, metallicSample);
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
        //float NdotV = max(dot(normal, viewDirection), 0.0000001);
        float NdotL = max(dot(normal, lightDir), 0.0000001);
        float HdotV = max(dot(halfwayDir, viewDirection), 0.0);
        float NdotH = max(dot(normal, halfwayDir), 0.0);
        
        float D = DistributionGGX(NdotH, roughnessSample);
        float G = GeometrySmith(NdotV, NdotL, roughnessSample);
        float3 F = FresnelSchlick(HdotV, baseReflectivity);
        
        float3 specular = D * G * F;
        specular /= 4.0 * NdotV * NdotL;
        
        float3 kD = float3(1.0, 1.0, 1.0) - F;
        kD *= 1.0 - metallicSample;
        
        directionalLightLuminance += (kD * albedoBufferSample / PI + specular) * radiance * NdotL;
    }
    
    // Spot Lights
    for (int s = 0; s < numPointLights; s++)
    {
        float3 lightDir = normalize(spotLights[s].position - positionBufferSample);
        float3 halfwayDir = normalize(lightDir + viewDirection);
        float distance = length(spotLights[s].position - positionBufferSample);
        float attenuation = 1.0 / (distance * distance);
        
        float theta = dot(lightDir, normalize(-spotLights[s].direction));
        float epsilon = spotLights[s].innerCutOff - spotLights[s].outerCutOff;
        float intensity = clamp((theta - spotLights[s].outerCutOff) / epsilon, 0.0, 1.0);

        float3 radiance = ((spotLights[s].diffuse * (spotLights[s].strength * 10000.0)) * intensity) * attenuation;
        
         // Cook-Torrance BRDF
        //float NdotV = max(dot(normal, viewDirection), 0.0000001);
        float NdotL = max(dot(normal, lightDir), 0.0000001);
        float HdotV = max(dot(halfwayDir, viewDirection), 0.0);
        float NdotH = max(dot(normal, halfwayDir), 0.0);
        
        float D = DistributionGGX(NdotH, roughnessSample);
        float G = GeometrySmith(NdotV, NdotL, roughnessSample);
        float3 F = FresnelSchlick(HdotV, baseReflectivity);
        
        float3 specular = D * G * F;
        specular /= 4.0 * NdotV * NdotL;
        
        float3 kD = float3(1.0, 1.0, 1.0) - F;
        kD *= 1.0 - metallicSample;
        
        spotLightLuminance += (kD * albedoBufferSample / PI + specular) * radiance * NdotL;
    }
    
    // Point Lights
    for (int p = 0; p < numPointLights; p++)
    {
        float3 lightDir = normalize(pointLights[p].position - positionBufferSample);
        float3 halfwayDir = normalize(viewDirection + lightDir);
        float distance = length(pointLights[p].position - positionBufferSample);
        float attenuation = 1.0 / (distance * distance);
        float3 radiance = ((pointLights[p].diffuse * 255.0) * pointLights[p].strength) * attenuation;
        
        // Cook-Torrance BRDF
        float NdotL = max(dot(normal, lightDir), 0.0000001);
        float HdotV = max(dot(halfwayDir, viewDirection), 0.0);
        float NdotH = max(dot(normal, halfwayDir), 0.0);
        
        float D = DistributionGGX(NdotH, roughnessSample);
        float G = GeometrySmith(NdotV, NdotL, roughnessSample);
        float3 F = FresnelSchlick(HdotV, baseReflectivity);
        
        float3 specular = D * G * F;
        specular /= 4.0 * NdotV * NdotL;
        
        float3 kD = float3(1.0, 1.0, 1.0) - F;
        kD *= 1.0 - metallicSample;
        
        pointLightLuminance += (kD * albedoBufferSample / PI + specular) * radiance * NdotL;
    }
    
    // IBL
    // Irradiance
    float3 F_IBL = FresnelSchlickRoughness(NdotV, baseReflectivity, roughnessSample);
    float3 kD_IBL = (1.0f - F_IBL) * (1.0f - metallicSample);
    float3 diffuse_IBL = tc_IrradianceMap.Sample(s_LinearWrapSampler, normal).rgb * albedoBufferSample * kD_IBL;
    // Specular IBL
    const float MAX_REFLECTION_LOD = 4.0f;
    float3 environmentMapColor = tc_EnvironmentMap.SampleLevel(s_LinearWrapSampler, reflect(-viewDirection, normal), roughnessSample * MAX_REFLECTION_LOD).rgb;
    float2 brdf = t_BrdfLUT.Sample(s_LinearWrapSampler, float2(NdotV, roughnessSample)).rg;
    float3 specular_IBL = environmentMapColor * (F_IBL * brdf.r + brdf.g);
    
    float3 ambient = (diffuse_IBL + specular_IBL) * aoSample;
    float3 outputLightLuminance = directionalLightLuminance + pointLightLuminance + spotLightLuminance;
    
     // Combine Light luminance
    float3 pixelColor = ambient + outputLightLuminance;
    
    ps_out.litImage.rgb = ambient + outputLightLuminance;
    return ps_out;
    //HDRToneMap(pixelColor);
    //GammaCorrect(pixelColor);
    //return float4(pixelColor, 1.0);
}

void HDRToneMap(inout float3 target)
{
    target = float3(1.0, 1.0, 1.0) - exp(-target * cameraExposure);
}

void GammaCorrect(inout float3 target)
{
    const float gamma = 2.2;
    target = pow(target.rgb, float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * cameraNearZ * cameraFarZ) / (cameraFarZ + cameraNearZ - z * (cameraFarZ - cameraNearZ)) / cameraFarZ;
}

