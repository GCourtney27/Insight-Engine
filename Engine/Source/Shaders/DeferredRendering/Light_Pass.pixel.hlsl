#include <Deferred_Rendering.hlsli>	
#include <../Common/PBR_Helper.hlsli>

Texture2D t_AlbedoGBuffer : register(t0);
Texture2D t_NormalGBuffer : register(t1);
Texture2D t_RoughnessMetallicAOGBuffer : register(t2);
Texture2D t_PositionGBuffer : register(t3);
Texture2D t_DepthGBuffer : register(t4);

sampler s_LinearWrapSampler : register(s0);

// Function signatures
// -------------------
float3 CalculatePointLight(PointLight light, float3 normal, float3 fragPosition, float3 viewDirection, float2 texCoords);
float LinearizeDepth(float depth);
float3 GammaCorrect(float3 target);

float4 main(PS_INPUT_LIGHTPASS ps_in) : SV_TARGET
{
	
    float3 albedoBufferSample = pow(t_AlbedoGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb, float3(2.2, 2.2, 2.2));
    float3 normalBufferSample = t_NormalGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    float3 t_RoughMetAOBufferSample = t_RoughnessMetallicAOGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    float3 positionBufferSample = t_PositionGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    float depthBufferSample = t_DepthGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).r;
    float roughnessSample = t_RoughMetAOBufferSample.r;
    float metallicSample = t_RoughMetAOBufferSample.g;
    float aoSample = t_RoughMetAOBufferSample.b;
    
    float3 normal = (normalBufferSample);
    //return float4(pointLights[0].diffuse, 1.0);
    float3 viewDirection = normalize(cameraPosition - positionBufferSample);
    float3 F0 = float3(0.04, 0.04, 0.04);
    float3 baseReflectivity = lerp(F0, albedoBufferSample, metallicSample);
    
    float3 directionalLightLuminance = float3(0.0, 0.0, 0.0);
    float3 pointLightLuminance = float3(0.0, 0.0, 0.0);
    
    // Calculate Light Radiance
    // Directional Lights
    for (int d = 0; d < numDirectionalLights; d++)
    {
        float3 lightDir = normalize(-dirLights[d].direction);
        float3 halfwayDir = normalize(viewDirection + lightDir);
        float3 radiance = (dirLights[d].diffuse * 255.0) * dirLights[d].strength;
        
          // Cook-Torrance BRDF
        float NdotV = max(dot(normal, viewDirection), 0.0000001);
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
    
    // Point Lights
    for (int p = 0; p < numPointLights; p++)
    {
        float3 lightDir = normalize(pointLights[p].position - positionBufferSample);
        float3 halfwayDir = normalize(viewDirection + lightDir);
        float distance = length(pointLights[p].position - positionBufferSample);
        float attenuation = 1.0 / (distance * distance);
        float3 radiance = ((pointLights[p].diffuse * 255.0) * pointLights[p].strength) * attenuation;
        
        // Cook-Torrance BRDF
        float NdotV = max(dot(normal, viewDirection), 0.0000001);
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
    
    float3 ambient = float3(0.03, 0.03, 0.03) * albedoBufferSample * aoSample; // TODO replace floa3 with IR map sample
    float3 outputLuminance = directionalLightLuminance + pointLightLuminance;
    float3 result = ambient * outputLuminance;
    
    // HDR Tonemapping
    float3 mapped = float3(1.0, 1.0, 1.0) - exp(-result * cameraExposure);
    mapped = GammaCorrect(mapped);
    return float4(mapped, 1.0);
}

float3 GammaCorrect(float3 target)
{
    const float gamma = 2.2;
    return pow(target.rgb, float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * cameraNearZ * cameraFarZ) / (cameraFarZ + cameraNearZ - z * (cameraFarZ - cameraNearZ)) / cameraFarZ;
}



