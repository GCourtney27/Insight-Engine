#include <../DeferredRendering/Deferred_Rendering.hlsli>
#define mod(x, y) (x - y * floor(x / y))
#define Scale float3(0.8, 0.8, 0.8)
#define K 19.19

// Texture Inputs
// --------------
Texture2D t_AlbedoGBuffer : register(t0);
Texture2D t_NormalGBuffer : register(t1);
Texture2D t_RoughnessMetallicAOGBuffer : register(t2);
Texture2D t_PositionGBuffer : register(t3);
Texture2D t_DepthGBuffer : register(t4);

Texture2D t_LightPassResult : register(t14);

// Samplers
// --------
SamplerState s_LinearWrapSampler : register(s1);

// Function Signatures
// -------------------
float3 AddFilmGrain(float3 sourceColor, float2 texCoords);
float3 AddVignette(float3 sourceColor, float2 texCoords);
void GammaCorrect(inout float3 target);
void HDRToneMap(inout float3 target);
float LinearizeDepth(float depth);

float4 RayCast(in float3 dir, inout float3 hitCoord, out float dDepth);
float3 BinarySearch(inout float3 dir, inout float3 hitCoord, inout float dDepth);
float3 FresnelSchlick(in float cosTheta, in float3 f0);
float3 hash(float3 a);

//static const float rayStep = 0.1;
//static const float minRayStep = 0.1;
//static const int maxRaySteps = 30.0;
//static const float searchDist = 5.0;
//static const int numBinarySearchSteps = 5.0;
//static const float reflectionSpecularFalloffExponent = 3.0;

struct PS_INPUT_POSTFX
{
    float4 sv_position : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

float4 main(PS_INPUT_POSTFX ps_in) : SV_TARGET
{
    //float3 result = t_LightPassResult.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    //float metallic = t_RoughnessMetallicAOGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).g;
    //float3 normalViewSpace = mul(t_NormalGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).xyzw, invView).xyz;
    //float3 viewPos = t_PositionGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).xyz;
    //float3 albedo = t_AlbedoGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    //float roughness = t_RoughnessMetallicAOGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).r;
    
    //float3 f0 = float4(0.4, 0.4, 0.4, 0.4);
    //f0 = lerp(f0, albedo, metallic);
    //float3 fresnel = FresnelSchlick(max(dot(normalize(normalViewSpace), normalize(viewPos)), 0.0), f0);
    
    //float3 reflected = normalize(reflect(normalize(viewPos), normalize(normalViewSpace)));
    //float3 hitPos = viewPos;
    //float dDepth;
    
    //float3 worldPos = float3(mul(float4(viewPos, 1.0), invView).xyz);
    //float3 jitt = lerp(float3(0.0, 0.0, 0.0), float3(hash(worldPos)), roughness);
    
    //float3 coords = RayCast(jitt + reflected * max(minRayStep, -viewPos.z), hitPos, dDepth);
    
    //float2 dCoords = smoothstep(0.2, 0.6, abs(float2(0.5, 0.5) - coords.xy));
    
    //float screenEdgeFactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
    
    //float multiplier = pow(metallic, reflectionSpecularFalloffExponent) * screenEdgeFactor * -reflected.z;
    
    //float3 ssr = t_LightPassResult.Sample(s_LinearWrapSampler, coords.xy).rgb * clamp(multiplier, 0.0, 0.9) * fresnel;
    
    
    float3 result = t_LightPassResult.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
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
    //return float4(ssr, 1.0);
}

//float3 hash(float3 a)
//{
//    a = frac(a * Scale);
//    a += dot(a, a.yxz + K);
//    return frac((a.xxy + a.yxx) * a.zyx);
//}

//float3 FresnelSchlick(in float cosTheta, in float3 f0)
//{
//    return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 0.5);
//}

//float4 RayCast(in float3 dir, inout float3 hitCoord, out float dDepth)
//{
//    dir *= rayStep;
//    float depth = 0.0;
//    int steps = 0.0;
//    float4 projectedCoord = float4(0.0, 0.0, 0.0, 0.0);
    
//    [unroll]
//    for (int i = 0; i < maxRaySteps; i++)
//    {
//        hitCoord += dir;
//        projectedCoord = mul(projection, float4(hitCoord, 1.0));
//        projectedCoord.xy /= projectedCoord.w;
//        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
        
//        depth = t_PositionGBuffer.Sample(s_LinearWrapSampler, projectedCoord.xy).z;
        
//        if (depth > 1000.0)
//        {
//            continue;
//        }
        
//        dDepth = hitCoord - depth;
        
//        if ((dir.z - depth) < 1.2)
//        {
//            if (depth <= 0.0)
//            {
//                float4 result = float4(BinarySearch(dir, hitCoord, dDepth), 1.0);
//            }
//            steps++;
//        }

//    }
    
//    return float4(projectedCoord.x, projectedCoord.y, depth, 1.0);

//}

//float3 BinarySearch(inout float3 dir, inout float3 hitCoord, inout float dDepth)
//{
//    float depth;
//    float4 projectedCoord;
    
//    [unroll]
//    for (int i = 0; i < numBinarySearchSteps; i++)
//    {
//        projectedCoord = mul(projection, float4(hitCoord, 1.0));
//        projectedCoord.xy /= projectedCoord.w;
//        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
//        depth = t_PositionGBuffer.Sample(s_LinearWrapSampler, projectedCoord.xy).z;
        
//        dDepth = hitCoord - depth;
        
//        dir += 0.5;
        
//        if (depth < 0.0)
//        {
//            hitCoord += dir;
//        }
//        else
//        {
//            hitCoord -= dir;
//        }

//    }
    
//    projectedCoord = mul(projection, float4(hitCoord, 1.0));
//    projectedCoord.xy /= projectedCoord.w;
//    projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
//    depth = t_PositionGBuffer.Sample(s_LinearWrapSampler, projectedCoord.xy).z;
    
//    return float3(projectedCoord.x, projectedCoord.y, depth);

//}

float3 AddFilmGrain(float3 sourceColor, float2 texCoords)
{

    float x = (texCoords.x + 4.0) * (texCoords.y + 4.0) * (time * 10.0);
    float grain = mod((mod(x, 13.0) + 1.0) * (mod(x, 123.0) + 1.0), 0.01) - 0.005;
    float4 grainAmount = float4(grain, grain, grain, grain) * fgStrength;
    
    grainAmount = 1.0 - grainAmount;
    return grainAmount.rgb * sourceColor;
}

float3 AddVignette(float3 sourceColor, float2 texCoords)
{
    float2 centerUV = texCoords - float2(0.5, 0.5);
    float3 color = float4(1.0, 1.0, 1.0, 1.0);

    color.rgb *= 1.0 - smoothstep(vnInnerRadius, vnOuterRadius, length(centerUV));
    color *= sourceColor;
    color = lerp(sourceColor, color, vnOpacity);
    
    return color;
}

void HDRToneMap(inout float3 target)
{
    target = float3(1.0, 1.0, 1.0) - exp(-target * cameraExposure);
}

void GammaCorrect(inout float3 target)
{
    const float gamma = 2.2;
    target = pow(target.rgb, float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * cameraNearZ * cameraFarZ) / (cameraFarZ + cameraNearZ - z * (cameraFarZ - cameraNearZ)) / cameraFarZ;
}

