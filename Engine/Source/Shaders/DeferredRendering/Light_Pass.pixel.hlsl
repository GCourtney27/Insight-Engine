#include <Deferred_Rendering.hlsli>	

Texture2D t_Albedo		: register(t0);
Texture2D t_Normal		: register(t1);
Texture2D t_Position	: register(t2);
Texture2D t_Depth		: register(t3);

sampler s_Sampler : register(s0);

// Function signatures
// -------------------
float3 CalculatePointLight(PointLight light, float3 normal, float3 fragPosition, float3 viewDirection, float2 texCoords);

float4 main(PS_INPUT_LIGHTPASS ps_in) : SV_TARGET
{
	float3 result = float3(0.0, 0.0, 0.0);
	
    float3 albedoSample    = t_Albedo.Sample(s_Sampler, ps_in.texCoords).rgb;
	float3 normalSample		= t_Normal.Sample(s_Sampler, ps_in.texCoords).rgb;
	float3 positionSample	= t_Position.Sample(s_Sampler, ps_in.texCoords).rgb;
    float depthSample		= t_Depth.Sample(s_Sampler, ps_in.texCoords).r;
    
    float3 normal = normalize(normalSample);
    float3 viewDirection = normalize(cameraPosition - positionSample);
    
    //const float3x3 tanToView = float3x3(normalize(ps_in.tangent),
    //                                     normalize(ps_in.biTangent),
    //                                     normalize(ps_in.normal));
	
    {
        result += CalculatePointLight(pointLights, normal, positionSample, viewDirection, ps_in.texCoords);
    }
    
    return float4(result, 1.0);
}

float3 CalculatePointLight(PointLight light, float3 normal, float3 fragPosition, float3 viewDirection, float2 texCoords)
{
    float3 lightDir = normalize(light.position - fragPosition);
    float3 halfwayDir = normalize(lightDir + viewDirection);
	
    float diffuseFactor = max(dot(lightDir, normal), 0.0);
    float specularFactor = pow(max(dot(normal, halfwayDir), 0.0), 8.0);
	
    float distance = length(light.position - fragPosition);
    float attenuation = 1.0 / (light.constantFactor + light.linearFactor * distance + light.quadraticFactor * (distance * distance));
	
    float3 textureColor = t_Albedo.Sample(s_Sampler, texCoords).rgb;
    float3 ambient = light.ambient * textureColor;
    float3 diffuse = light.diffuse * diffuseFactor * textureColor;
    float3 specular = light.specular * specularFactor * textureColor;
	
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
	
    return (ambient + diffuse + specular);

}
