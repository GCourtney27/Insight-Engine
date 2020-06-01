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
    
    float3 viewDirection = normalize(cameraPosition - positionBufferSample);
    float3 F0 = float3(0.04, 0.04, 0.04);
    float3 baseReflectivity = lerp(F0, albedoBufferSample, metallicSample);
    
    float LO = float3(0.0, 0.0, 0.0);
    
    // Calculate Light Radiance
    //result += CalculateDirectionalLight(dirLight, normal, viewDirection, ps_in.texCoords);
    for (int i = 0; i < numPointLights; i++)
    {
        float3 lightDir = normalize(pointLights[i].position - positionBufferSample);
        float3 halfwayDir = normalize(viewDirection + lightDir);
        float distance = length(pointLights[i].position - positionBufferSample);
        float attenuation = 1.0 / (distance * distance);
        float3 radiance = pointLights[i].diffuse * attenuation;
        
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
        
        LO += (kD * albedoBufferSample / PI + specular) * radiance * NdotL;
    }
    
    float3 ambient = float3(0.3, 0.3, 0.3) * albedoBufferSample * aoSample;
    float3 result = ambient * LO;
    
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



