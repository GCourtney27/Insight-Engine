
cbuffer lightBuffer : register(b0)
{
	float3 ambientLightColor;
	float ambientLightStrength;

	float3 dynamicLightColor;
	float dynamicLightStrength;
	float3 dynamicLightPosition;

	float dynamicLightAttenuation_a;
	float dynamicLightAttenuation_b;
	float dynamicLightAttenuation_c;
}

struct PS_INPUT
{
	float4 inPosition : SV_POSITION; // Screen space pixel position
	float2 inTexCoord_bc : BASECOLOR;
	float2 inTexCoord_o : OPACITY;
	float3 inNormal : NORMAL;
	float3 inWorldPos : WORLD_POSITION; // World space pixel position
};

Texture2D objTexture_bc : ALBEDO: register(t0);
Texture2D objTexture_o : OPACITY: register(t1);
SamplerState objSamplerState : SAMPLER: register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 sampleColor = objTexture_bc.Sample(objSamplerState, input.inTexCoord_bc);
	float3 sampleOpactiy = objTexture_o.Sample(objSamplerState, input.inTexCoord_o);

	/*float opacity = 0.0f;
	if (sampleOpactiy.x = 1.0f && sampleOpactiy.y = 1.0f && sampleOpactiy.y = 1.0f)
		opacity = 1.0f;
	else
		opacity = 0.0f;*/
	//float3 sampleColor = input.inNormal; // Show the normal maps on the objects(Kind of cool)

	float3 ambientLight = ambientLightColor * ambientLightStrength;

	float3 appliedLight = ambientLight;

	float3 vectorToLight = normalize(dynamicLightPosition - input.inWorldPos);

	float3 diffuseLightIntensity = max(dot(vectorToLight, input.inNormal), 0);

	float distanceToLight = distance(dynamicLightPosition, input.inWorldPos);

	float attenuationFactor = 1 / (dynamicLightAttenuation_a + dynamicLightAttenuation_b * distanceToLight + dynamicLightAttenuation_c * pow(distanceToLight, 2));

	diffuseLightIntensity *= attenuationFactor;

	float3 diffuseLight = diffuseLightIntensity * dynamicLightStrength * dynamicLightColor;

	appliedLight += diffuseLight;

	float3 finalColor = sampleColor * saturate(appliedLight);

	return float4(finalColor, 1.0f);
}