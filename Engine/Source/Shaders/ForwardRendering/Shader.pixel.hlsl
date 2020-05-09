
Texture2D tAlbedo : register(t0);
Texture2D tNormal : register(t1);

SamplerState sampler1 : register(s0);

struct PS_INPUT
{
    float4 position  : SV_POSITION;
    float2 texCoords : TEXCOORD;
    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
    float3 biTangent : BITANGENT;
};

cbuffer cbPerFrame :register(b1)
{
    float3 cameraPosition;
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float4 albedo = tAlbedo.Sample(sampler1, ps_in.texCoords);
    float4 normal = tNormal.Sample(sampler1, ps_in.texCoords);
    
	float3 viewDir = normalize(cameraPosition - ps_in.position.xyz);
    
	float3 lightDir = normalize(-float3(1.0, 1.0, 0.0));
	float3 halfwayDir = normalize(lightDir + viewDir);
    
	float3 diffuseFactor = max(dot(ps_in.normal, lightDir), 0.0);
	float specularFactor = pow(max(dot(ps_in.normal, halfwayDir), 0.0), 8.0);
    
	float3 ambient = float3(1.0, 1.0, 1.0) * albedo.rgb;
	float3 diffuse = float3(1.0, 1.0, 1.0) * diffuseFactor * albedo.rgb;
	//float3 specular = float3(1.0, 1.0, 1.0) * specularFactor * 
    
	return float4((ambient * diffuse * float3(1.0, 1.0, 1.0)), 1.0);
    
    return albedo;
    
    //float4 color = {ps_in.normal, 1.0f};
    //return color;

}