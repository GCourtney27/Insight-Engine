
Texture2D tAlbedo : register(t0);
Texture2D tNormal : register(t1);
Texture2D tSpecular : register(t2);

SamplerState sampler1 : register(s0);

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texCoords : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 biTangent : BITANGENT;
};

cbuffer cbPerFrame : register(b1)
{
	float3 cameraPosition;
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
	return float4(1.0, 0.0, 0.0, 1.0);
	
	float3 albedoSample = tAlbedo.Sample(sampler1, ps_in.texCoords).rgb;
	float3 normalSample = tNormal.Sample(sampler1, ps_in.texCoords).rgb;
	float3 specularSample = tSpecular.Sample(sampler1, ps_in.texCoords).rgb;
	ps_in.normal = normalize(ps_in.normal * 2.0 - 1.0);
    
	float3 viewDir = normalize(cameraPosition - ps_in.position.xyz);
    
	float3 lightDir = normalize(-float3(0.0, 10.0, -10.0));
	float3 halfwayDir = normalize(lightDir + viewDir);
    
	float3 diffuseIntensity = max(dot(ps_in.normal, lightDir), 0.0);
	float specularIntensity = pow(max(dot(ps_in.normal, halfwayDir), 0.0), 16.0);
    
	float3 ambient = float3(0.3, 0.3, 0.3) * albedoSample.rgb;
	float3 diffuse = float3(1.0, 1.0, 1.0) * diffuseIntensity * albedoSample.rgb;
	float3 specular = float3(1.0, 1.0, 1.0) * specularIntensity * specularSample.rgb;
    
	float3 result = (ambient * (diffuse + specular) * float3(1.0, 1.0, 1.0));
	
	return float4(result, 1.0);
}