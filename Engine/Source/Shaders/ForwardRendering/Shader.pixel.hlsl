
Texture2D t_Albedo	  : register(t0);
Texture2D t_Normal	  : register(t1);
Texture2D t_Roughness  : register(t2);
Texture2D t_MMetallic  : register(t3);
Texture2D t_Specular	  : register(t4);
Texture2D t_AO		  : register(t5);

SamplerState sampler1 : register(s0);

struct PS_INPUT
{
	float4 position		: SV_POSITION;
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

float4 main(PS_INPUT ps_in) : SV_TARGET
{
	//return float4(ps_in.FragPos, 1.0);
	//return float4(1.0, 0.0, 0.0, 1.0);
	
	
	float3 albedoSample = t_Albedo.Sample(sampler1, ps_in.texCoords).rgb;
	float3 normalSample = t_Normal.Sample(sampler1, ps_in.texCoords).rgb;
	float3 specularSample = t_Specular.Sample(sampler1, ps_in.texCoords).rgb;
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
	
	
	float3 viewDir = normalize(cameraPosition - ps_in.FragPos);
    
	float3 lightDir = normalize(-float3(0.0, 10.0, -10.0));
	float3 halfwayDir = normalize(lightDir + viewDir);
    
	float3 diffuseIntensity = max(dot(normal, lightDir), 0.0);
	float specularIntensity = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    
	float3 ambient = float3(1.0, 1.0, 1.0) * albedoSample.rgb;
	float3 diffuse = float3(1.0, 1.0, 1.0) * diffuseIntensity * albedoSample.rgb;
	float3 specular = float3(1.0, 1.0, 1.0) * specularIntensity * specularSample.rgb;
    
	float3 result = (ambient * (diffuse + specular) * float3(1.0, 1.0, 1.0));
	
	return float4(result, 1.0);
}