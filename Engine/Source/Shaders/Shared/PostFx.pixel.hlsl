#include <../DeferredRendering/Deferred_Rendering.hlsli>

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
void AddVignette(inout float3 sourceColor, float2 texCoords);
void GammaCorrect(inout float3 target);
void HDRToneMap(inout float3 target);
float LinearizeDepth(float depth);

struct PS_INPUT_POSTFX
{
    float4 sv_position : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

float4 main(PS_INPUT_POSTFX ps_in) : SV_TARGET
{
    float3 result = t_LightPassResult.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    
    AddVignette(result, ps_in.texCoords);
    
    HDRToneMap(result);
    GammaCorrect(result);
    return float4(result, 1.0);
}

void AddVignette(inout float3 sourceColor, float2 texCoords)
{
    float2 centerUV = texCoords - float2(0.5, 0.5);
    float3 color = float4(1.0, 1.0, 1.0, 1.0);

    color.rgb *= 1.0 - smoothstep(vnInnerRadius, vnOuterRadius, length(centerUV));
    color *= sourceColor;
    color = lerp(sourceColor, color, vnOpacity);
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

