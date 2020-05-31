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
float3 CalculateDirectionalLight(DirectionalLight light, float3 normal, float3 viewDirection, float2 texCoords);
float LinearizeDepth(float depth);
float3 GammaCorrect(float3 target);

float4 main(PS_INPUT_LIGHTPASS ps_in) : SV_TARGET
{
    float3 result = float3(0.0, 0.0, 0.0);
	
    float3 albedoBufferSample = t_AlbedoGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    float3 normalBufferSample = t_NormalGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    float3 t_RoughMetAOBufferSample = t_RoughnessMetallicAOGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    float3 positionBufferSample = t_PositionGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    float depthBufferSample = t_DepthGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).r;
    
    float roughnessSample = t_RoughMetAOBufferSample.r;
    float metallicSample = t_RoughMetAOBufferSample.g;
    float aoSample = t_RoughMetAOBufferSample.b;
    
    float z = LinearizeDepth(depthBufferSample) / cameraFarZ;
    
    float3 normal = (normalBufferSample);
    float3 viewDirection = normalize(cameraPosition - positionBufferSample);
    
    // Calculate Radiance
    //result += CalculateDirectionalLight(dirLight, normal, viewDirection, ps_in.texCoords);
    for (int i = 0; i < MAX_POINT_LIGHTS_SUPPORTED; i++)
    {
        result += CalculatePointLight(pointLights[i], normal, positionBufferSample, viewDirection, ps_in.texCoords);
    }
    
    // PBR
    {
        // Cook-Torrance BRDF
        //float NdotV = max(dot(normal, viewDirection), 0.0000001);
    
        //float P_NdotL = max(dot(normal, P_L), 0.0000001);
        //float P_HdotV = max(dot(P_H, V), 0.0f);
        //float P_NdotH = max(dot(N, P_H), 0.0f);
    
        //float P_D = distributionGGX(P_NdotH, roughnessSample);
        //float P_G = geometrySmith(NdotV, P_NdotL, roughnessSample);
        //float3 P_F = fresnelSchlick(P_HdotV, baseReflectivity);
    
        //float3 P_specular = P_D * P_G * P_F;
        //P_specular /= 4.0f * NdotV * P_NdotL;

        //float3 P_kD = float3(1.0f, 1.0f, 1.0f) - P_F;
    
        //P_kD *= 1.0f - metallicSample;

        //Lo += (P_kD * albedoSample / PI + P_specular) * pointLightRadiance * P_NdotL;
    
        //float Dir_NdotH = max(dot(N, Dir_H), 0.0f);
        //float Dir_HdotV = max(dot(Dir_H, V), 0.0f);
        //float Dir_NdotL = max(dot(N, Dir_L), 0.0000001);

        //float Dir_D = distributionGGX(Dir_NdotH, roughnessSample);
        //float Dir_G = geometrySmith(NdotV, Dir_NdotL, roughnessSample);
        //float3 Dir_F = fresnelSchlick(Dir_HdotV, baseReflectivity);

        //float3 Dir_specular = Dir_D * Dir_G * Dir_F;
        //Dir_specular /= 4.0f * NdotV * Dir_NdotL;

        //float3 Dir_kD = float3(1.0f, 1.0f, 1.0f) - Dir_F;
    
        //Dir_kD *= 1.0f - metallicSample;

        //Lo += (Dir_kD * albedoSample / PI + Dir_specular) * directionalLightRadiance * Dir_NdotL;
    }
    
    result = GammaCorrect(result);
    return float4(result, 1.0);
}

float3 GammaCorrect(float3 target)
{
    const float gamma = 2.2;
    return pow(target.rgb, float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * cameraNearZ * cameraFarZ) / (cameraFarZ + cameraNearZ - z * (cameraFarZ - cameraNearZ));
}

// TODO: Fix this its amplifyingred values
float3 CalculateDirectionalLight(DirectionalLight light, float3 normal, float3 viewDirection, float2 texCoords)
{
    float3 lightDir = normalize(-light.direction);
    float3 halfwayDir = normalize(lightDir + viewDirection);
    
    float diffuseFactor = max(dot(normal, lightDir), 0.0);
    float specularFactor = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    
    float3 textureColor = t_AlbedoGBuffer.Sample(s_LinearWrapSampler, texCoords).rgb;
    float3 ambient = light.ambient * textureColor;
    float3 diffuse = light.diffuse * diffuseFactor * textureColor;
    float3 specular = specularFactor * t_RoughnessMetallicAOGBuffer.Sample(s_LinearWrapSampler, texCoords).r;

    return (ambient + diffuse + specular);
}

float3 CalculatePointLight(PointLight light, float3 normal, float3 fragPosition, float3 viewDirection, float2 texCoords)
{
    float3 lightDir = normalize(light.position - fragPosition);
    float3 halfwayDir = normalize(lightDir + viewDirection);
	
    float diffuseFactor = max(dot(lightDir, normal), 0.0);
    float specularFactor = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
	
    float distance = length(light.position - fragPosition);
    float attenuation = 1.0 / (light.constantFactor + light.linearFactor * distance + light.quadraticFactor * (distance * distance));
	
    float3 textureColor = t_AlbedoGBuffer.Sample(s_LinearWrapSampler, texCoords).rgb;
    float3 ambient = light.ambient * textureColor;
    float3 diffuse = light.diffuse * diffuseFactor * textureColor;
    float3 specular = specularFactor * 16.0;
	
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
	
    return (ambient + diffuse + specular);

}
