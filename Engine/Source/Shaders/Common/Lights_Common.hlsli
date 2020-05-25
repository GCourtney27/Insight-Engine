
struct PointLight
{
	float3 position;
	float3 color;
	
	float linearFactor;
	float constantFactor;
	float quadraticFactor;
	
	float3 ambient;
	float3 diffuse;
	float3 specular;
};