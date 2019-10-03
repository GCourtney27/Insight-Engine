SamplerState gSampler;

Texture2D gTexDiffuse;
Texture2D gTexNormal;
Texture2D gTexPosition;

cbuffer cbPerObject
{
	float4x4 gModel;
	float4x4 gView;
	float4x4 gProjection;
};

struct VertexIn
{
	float3 position : POSITION;
	float3 normal	: NORMAL;
	float2 tex		: TEXCOORD;
};

struct VertexOut
{
	float4 hPos		: HPOS;
	float4 position : SV_POSITION;
	float3 normal	: NORMAL;
	float2 tex		: TEXCOORD0;
};

struct PixelOutDeferred
{
	float4 diffuse  : SV_Target0;
	float4 normal   : SV_Target1;
	float4 position : SV_Target2;
};

struct PixelOutBasic
{
	float4 color : SV_Target;
};


VertexOut BasicVS(VertexIn vIn)
{
	VertexOut vOut;
	vOut.hPos = float4(vIn.position, 1.0f);
	vOut.position = mul(vOut.hPos, mul(gModel, mul(gView, gProjection)));
	vOut.normal = mul(normalize(vIn.normal), (float3x3)gModel);
	vOut.tex = vIn.tex;

	return vOut;
}

PixelOutDeferred BasicPS(VertexOut pIn)
{
	PixelOutDeferred pOut;
	pOut.diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	pOut.normal = float4(pIn.normal, 1.0f);
	pOut.position = mul(pIn.hPos, gModel);
	return pOut;
}

VertexOut LightingVS(VertexIn vIn)
{
	VertexOut vOut;
	vOut.hPos = float4(vIn.position, 1.0f);
	vOut.position = vOut.hPos;
	vOut.tex = vIn.tex;

	return vOut;
}

PixelOutBasic LightingPS(VertexOut pIn)
{
	PixelOutBasic pOut;
	float3 normal = gTexNormal.Sample(gSampler, pIn.tex).rgb;
	float3 diffuse = gTexDiffuse.Sample(gSampler, pIn.tex).rgb;

	if (length(normal) > 0.0f)
	{
		float3 lightDir = normalize(float3(1.0f, 1.0f, 1.0f));
		float3 position = float3(gView._41, gView._42, gView._43);

		float lambertian = max(dot(lightDir, normal), 0.0f);
		float specular = 0.0f;

		[flatten]
		if (lambertian > 0.0f)
		{
			float3 viewDir = normalize(-position);
			float3 halfDir = normalize(lightDir + viewDir);
			float specAngle = max(dot(halfDir, normal), 0.0f);
			specular = pow(specAngle, 100.0f);
		}

		float3 colorLinear = lambertian * diffuse + specular * float3(1.0f, 1.0f, 1.0f);
		pOut.color = float4(pow(colorLinear, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f)), 1.0f);
		return pOut;
	}

	pOut.color = float4(diffuse, 1.0f);
	return pOut;
}

technique11 BasicTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, BasicVS()));
		SetPixelShader(CompileShader(ps_5_0, BasicPS()));
	}
};

technique11 LightingTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, LightingVS()));
		SetPixelShader(CompileShader(ps_5_0, LightingPS()));
	}
};