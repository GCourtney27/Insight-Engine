#include <../Common/InsightCommon.hlsli>
#include <..//DeferredRendering/DeferredRendering.hlsli>

// Texture Inputs
// --------------
Texture2D t_AlbedoGBuffer : register(t0);
Texture2D t_NormalGBuffer : register(t1);
Texture2D t_RoughnessMetallicAOGBuffer : register(t2);
Texture2D t_PositionGBuffer : register(t3);
Texture2D t_SceneDepthGBuffer : register(t4);

Texture2D t_ShadowDepthPass : register(t10);
Texture2D t_LightPassResult : register(t15);

RWTexture2D<float4> rw_BloomPassResult : register(u1);

// Samplers
// --------
sampler s_PointClampSampler : register(s0);
sampler s_LinearWrapSampler : register(s1);

// Function Signatures
// -------------------
float3 AddFilmGrain(float3 sourceColor, float2 texCoords);
float3 AddVignette(float3 sourceColor, float2 texCoords);
float3 AddChromaticAberration(float3 sourceColor, float2 texCoords);
float3 AddBloom(float3 sourceColor, float2 texCoords);
void LinearizeDepth(inout float depth);


void HDRToneMap(inout float3 target);
void GammaCorrect(inout float3 target);


// Pixel Shader Return Value
// -------------------------
struct PS_INPUT_POSTFX
{
	float4 sv_position : SV_POSITION;
	float2 texCoords : TEXCOORD;
};

float4 main(PS_INPUT_POSTFX ps_in) : SV_TARGET
{
	float3 LightPassResult = t_LightPassResult.Sample(s_PointClampSampler, ps_in.texCoords).rgb;
	
    
    float3 result = LightPassResult;
	
    if (caEnabled)
    {
        result = AddChromaticAberration(result, ps_in.texCoords);
    }
	if (blEnabled)
	{
		result = AddBloom(result, ps_in.texCoords);
	}
	if (vnEnabled)
	{
		result = AddVignette(result, ps_in.texCoords);
	}
	if (fgEnabled)
	{
		result = AddFilmGrain(result, ps_in.texCoords);
	}
    
	HDRToneMap(result);
	GammaCorrect(result);
    
	return float4(result, 1.0);
}

void HDRToneMap(inout float3 target)
{
	target = float3(1.0, 1.0, 1.0) - exp(-target * cbCameraExposure);
}

void GammaCorrect(inout float3 target)
{
	const float gamma = 2.2;
	target = pow(target.rgb, float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));
}

float mod(float x, float y)
{
	return (x - y * floor(x / y));
}

float3 AddBloom(float3 sourceColor, float2 texCoords)
{	
    float2 PixelCoords = (texCoords) * (cbScreenSize / 2.0);
    //float2 PixelCoords = (texCoords * cbScreenSize) * 2;
    float4 BloomPassResult = rw_BloomPassResult[PixelCoords];
    //return BloomPassResult.rgb;
    return mad(blCombineCoefficient, sourceColor, BloomPassResult.rgb);
}

float3 AddChromaticAberration(float3 sourceColor, float2 texCoords)
{
	float2 texel = 1.0 / cbScreenSize;
    float2 coords = (texCoords - 0.5) * 2.0;
	float coordsDot = dot(coords, coords);
    
	float2 precompute = caIntensity * coordsDot * coords;
	float2 uvR = texCoords - texel.xy * precompute;
	float2 uvB = texCoords + texel.xy * precompute;
    
	sourceColor.r += t_LightPassResult.Sample(s_LinearWrapSampler, uvR).r;
	sourceColor.g += t_LightPassResult.Sample(s_LinearWrapSampler, texCoords).g;
	sourceColor.b += t_LightPassResult.Sample(s_LinearWrapSampler, uvB).b;
    
	return sourceColor;
}

float3 AddFilmGrain(float3 sourceColor, float2 texCoords)
{
	float x = (texCoords.x + 4.0) * (texCoords.y + 4.0) * (cbTime * 10.0);
	float grain = mod((mod(x, 13.0) + 1.0) * (mod(x, 123.0) + 1.0), 0.01) - 0.005;
	float4 grainAmount = float4(grain, grain, grain, grain) * fgStrength;
    
	grainAmount = 1.0 - grainAmount;
	return grainAmount.rgb * sourceColor;
}

float3 AddVignette(float3 sourceColor, float2 texCoords)
{
	float2 centerUV = texCoords - float2(0.5, 0.5);
	float3 color = float3(1.0, 1.0, 1.0);

	color.rgb *= 1.0 - smoothstep(vnInnerRadius, vnOuterRadius, length(centerUV));
	color *= sourceColor;
	color = lerp(sourceColor, color, vnOpacity);
    
	return color;
}

void LinearizeDepth(inout float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC 
	depth = (2.0 * cbCameraNearZ * cbCameraFarZ) / (cbCameraFarZ + cbCameraNearZ - z * (cbCameraFarZ - cbCameraNearZ)) / cbCameraFarZ;
}


float3 GetPositionViewSpace(float2 texCoord)
{
    float3 Pos = t_PositionGBuffer.Sample(s_PointClampSampler, texCoord).xyz;
    return mul(float4(Pos, 1.0), cbView).xyz;
}

#define getDepth(texCoord) t_SceneDepthGBuffer.Sample(s_PointClampSampler, texCoord).xyz

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 hash(float3 a)
{
    const float3 Scale = float3(0.8, 0.8, 0.8);
    const float K = 19.19;
    a = frac(a * Scale);
    a += dot(a, a.yxz + K);
    return frac((a.xxy + a.yxx) * a.zyx);
}

float2 BinarySearch(inout float3 dir, inout float3 hitCoord, inout float dDepth)
{
    const int numBinarySearchSteps = 10;
    
    float depth;
    float4 projectedCoord;
    
    for (int i = 0; i < numBinarySearchSteps; i++)
    {
        projectedCoord = mul(float4(hitCoord, 1.0), cbProjection);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
        

        //depth = GetPositionViewSpace(projectedCoord.xy).z;
        depth = getDepth(projectedCoord.xy).r;
        //LinearizeDepth(depth);
        
        dDepth = hitCoord.z - depth;

        dir *= 0.5;
        
        if (dDepth > 0.0)
            hitCoord += dir;
        else
            hitCoord -= dir;
    }
    
    //projectedCoord = mul(float4(hitCoord, 1.0), cbProjection);
    //projectedCoord.xy /= projectedCoord.w;
    //projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
    return float2(projectedCoord.xy);
}

float2 RayMarch(float3 dir, inout float3 hitCoord, out float dDepth)
{
    const float step = 0.05;
    const float maxSteps = 30;
    
    dir *= step;
  
    for (int i = 0; i < maxSteps; i++)
    {
        hitCoord *= (step);
        //hitCoord *= dir;
 
        float4 projectedCoord = mul(float4(hitCoord, 1.0), cbProjection);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        //float depth = GetPositionViewSpace(projectedCoord.xy).z;
        float depth = getDepth(projectedCoord.xy).r;
        //LinearizeDepth(depth);
       
        dDepth = hitCoord.z - depth;

        if ((dir.z - dDepth) < 1.2 && dDepth <= 0.0)
        {
            return BinarySearch(dir, hitCoord, dDepth);
        }
    }
 
    return float2(-1.0, -1.0);
}

#define fresnelExp 5.0
float fresnel(float3 direction, float3 normal)
{
    float3 halfDirection = normalize(normal + direction);
    
    float cosine = dot(halfDirection, direction);
    float product = max(cosine, 0.0);
    float factor = 1.0 - pow(product, fresnelExp);
    
    return factor;
}

float3 SSR(float2 TexCoords)
{
    float Metallic = t_RoughnessMetallicAOGBuffer.Sample(s_LinearWrapSampler, TexCoords).b;
    float Roughness = t_RoughnessMetallicAOGBuffer.Sample(s_LinearWrapSampler, TexCoords).r;
    float Spec = t_RoughnessMetallicAOGBuffer.Sample(s_LinearWrapSampler, TexCoords).a;
    float3 Albedo = t_AlbedoGBuffer.Sample(s_LinearWrapSampler, TexCoords).rgb;
    
    float3 ViewNormal = t_NormalGBuffer.Sample(s_LinearWrapSampler, TexCoords).rgb; // View space
    ViewNormal = mul(float4(ViewNormal, 1.0), cbInverseView).xyz;
    float3 ViewPosition = GetPositionViewSpace(TexCoords); // View space
    
    float3 WorldPosition = mul(float4(ViewPosition, 1.0), cbInverseView).xyz;
    float3 jitt = hash(WorldPosition) * Roughness;
	
    // Reflection vector
    float3 reflected = normalize(reflect(normalize(ViewPosition), normalize(ViewNormal)));
    
    float3 hitPos = ViewPosition;
    float dDepth;
    const float minRayStep = 0.2;
    float2 coords = RayMarch(jitt + reflected * max(-ViewPosition.z, minRayStep), hitPos, dDepth);
     
    float LLimiter = 0.1;
    float L = length(GetPositionViewSpace(coords) - ViewPosition);
    L = clamp(L * LLimiter, 0, 1);
    float error = 1 - L;

    float f = fresnel(reflected, ViewNormal);
    
    
    float3 SSR = t_AlbedoGBuffer.Sample(s_PointClampSampler, coords.xy).rgb /** error * f*/;
    float3 SSRResult;
    
    if (coords.x != -1.0 && coords.y != -1.0)
    {
        SSRResult = lerp(t_AlbedoGBuffer.Sample(s_PointClampSampler, TexCoords), float4(SSR, 1.0), Spec).rgb;
    }
    else
    {
        SSRResult = lerp(t_AlbedoGBuffer.Sample(s_PointClampSampler, TexCoords), float4(0.0, 0.0, 1.0, 1.0), Spec).rgb;
    }
    return SSRResult;
}