#include <Deferred_Rendering.hlsli>	

Texture2D t_AlbedoGBuffer		        : register(t0);
Texture2D t_NormalGBuffer		        : register(t1);
Texture2D t_RoughnessMetallicAOGBuffer  : register(t2);
Texture2D t_PositionGBuffer	            : register(t3);
Texture2D t_DepthGBuffer                : register(t4);

sampler s_LinearWrapSampler : register(s0);

// Function signatures
// -------------------
float3 CalculatePointLight(PointLight light, float3 normal, float3 fragPosition, float3 viewDirection, float2 texCoords);
float LinearizeDepth(float depth);
float3 GammaCorrect(float3 target);

float4 main(PS_INPUT_LIGHTPASS ps_in) : SV_TARGET
{
	float3 result = float3(0.0, 0.0, 0.0);
	
    float3 albedoBufferSample     = t_AlbedoGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    float3 normalBufferSample     = t_NormalGBuffer.Sample(s_LinearWrapSampler , ps_in.texCoords).rgb;
    float3 t_RoughnessMetallicAOGBufferSample = t_RoughnessMetallicAOGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    float3 positionBufferSample   = t_PositionGBuffer.Sample(s_LinearWrapSampler , ps_in.texCoords).rgb;
    float depthBufferSample       = t_DepthGBuffer.Sample(s_LinearWrapSampler , ps_in.texCoords).r;
    
    float roughnessSample = t_RoughnessMetallicAOGBufferSample.r;
    float metallicSample = t_RoughnessMetallicAOGBufferSample.g;
    float aoSample = t_RoughnessMetallicAOGBufferSample.b;
    
    float z = LinearizeDepth(depthBufferSample) / cameraFarZ;
    return float4(albedoBufferSample, 1.0);
    return float4(z, z, z, 1.0);
    
    float3 normal = (normalBufferSample);
    float3 viewDirection = normalize(cameraPosition - positionBufferSample);
    
    // Calculate PointLight Radiance
    for (int i = 0; i < MAX_POINT_LIGHTS_SUPPORTED; i++)
    {
        result += CalculatePointLight(pointLights[i], normal, positionBufferSample, viewDirection, ps_in.texCoords);
    }
    
	
    //float3 mapped = float3(1.0) - pow(-result.rgb * cameraExposure);
    //mapped = pow(mapped, float3(1.0 / gamma));
    
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

float3 CalculatePointLight(PointLight light, float3 normal, float3 fragPosition, float3 viewDirection, float2 texCoords)
{
    float3 lightDir = normalize(light.position - fragPosition);
    float3 halfwayDir = normalize(lightDir + viewDirection);
	
    float diffuseFactor = max(dot(lightDir, normal), 0.0);
    float specularFactor = pow(max(dot(normal, halfwayDir), 0.0), 16.0); // TODO load in shininess from texture
	
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
