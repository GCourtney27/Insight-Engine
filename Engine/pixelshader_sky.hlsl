struct SKY_PS_INPUT
{
	float4 inPosition : SV_POSITION;
	float3 inTexCoord : TEXCOORD;
};

TextureCube skyMap : SKY: register(t0);
//Texture2D objTexture : TEXTURE: register(t0);

SamplerState objSamplerState : SAMPLER: register(s0);

float4 main(SKY_PS_INPUT input) : SV_TARGET
{
	float4 result = skyMap.Sample(objSamplerState, input.inTexCoord);
	//float4 res = {1.0f, 1.0f, 0.0f, 1.0f};
	return result;
}