#define MAX_POINT_LIGHTS 4
#include <../Common/Lights_Common.hlsli>

Texture2D t_Albedo	  : register(t0);
Texture2D t_Normal	  : register(t1);
Texture2D t_Roughness : register(t2);
Texture2D t_MMetallic : register(t3);
Texture2D t_Specular  : register(t4);
Texture2D t_AO		  : register(t5);

Texture2D t_SkyDiffuse		   : register(t6);
Texture2D t_SkyIrradiance	   : register(t7);
Texture2D t_SkyEnvironmentMap  : register(t8);
Texture2D t_SkyBRDFLUT		   : register(t9);

SamplerState s_Sampler : register(s1);

struct PS_INPUT
{
	float4 sv_position	: SV_POSITION;
	float3 FragPos		: FRAG_POS;
	float2 texCoords	: TEXCOORD;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 biTangent	: BITANGENT;
};


cbuffer cbPerFrame : register(b1)
{
	float3 cameraPosition;
};

cbuffer cbLights : register(b2)
{
	PointLight pointLights;//[MAX_POINT_LIGHTS];
};

// Function Signatures
// -------------------
float3 CalculatePointLight(PointLight light, float3 normal, float3 fragPosition, float3 viewDirection, float2 texCoords);

float4 main(PS_INPUT ps_in) : SV_TARGET
{
	//return float4(ps_in.FragPos, 1.0);
	//return float4(1.0, 0.0, 0.0, 1.0);
	float3 result = float3(0.0, 0.0, 0.0);
	float3 normalSample = t_Normal.Sample(s_Sampler, ps_in.texCoords).rgb;
	float3 specularSample = t_Specular.Sample(s_Sampler, ps_in.texCoords).rgb;
	//float3 normal = normalize(ps_in.normal);// * 2.0 - 1.0);
    
	// Transform Normals From Tangent Space to View Space
	const float3x3 tanToView = float3x3(normalize(ps_in.tangent),
                                         normalize(ps_in.biTangent),
                                         normalize(ps_in.normal));
	float3 normal;
	normal.x = normalSample.x * 2.0f - 1.0f;
	normal.y = -normalSample.y * 2.0f + 1.0f;
	normal.z = normalSample.z;
	normal = normalize(mul(normal, tanToView));
	float3 viewDirection = normalize(cameraPosition - ps_in.FragPos);
	
	//for(int i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		//result += CalculatePointLight(pointLights, normal, ps_in.FragPos, viewDirection, ps_in.texCoords);
	}
	
	return float4(result, 1.0);
}

//float3 CalculatePointLight(PointLight light, float3 normal, float3 fragPosition, float3 viewDirection, float2 texCoords)
//{
//	float3 lightDir = normalize(light.position - fragPosition);
//	float3 halfwayDir = normalize(lightDir + viewDirection);
	
//	float diffuseFactor = max(dot(lightDir, normal), 0.0);
//	float specularFactor = pow(max(dot(normal, halfwayDir), 0.0), 8.0);
	
//	float distance = length(light.position - fragPosition);
//	float attenuation = 1.0 / (light.constantFactor + light.linearFactor * distance + light.quadraticFactor * (distance *distance));
	
//	float3 textureColor = t_Albedo.Sample(s_Sampler, texCoords).rgb;
//	float3 ambient = light.ambient * textureColor;
//	float3 diffuse = light.diffuse * diffuseFactor * textureColor;
//	float3 specular = light.specular * specularFactor * textureColor;
	
//	ambient *= attenuation;
//	diffuse *= attenuation;
//	specular *= attenuation;
	
//	return (ambient + diffuse + specular);

//}
