#include <Deferred_Rendering.hlsli>	

#define SHADOW_DEPTH_BIAS 0.00005f

// G-Buffer Inputs
// --------------
Texture2D t_AlbedoGBuffer               : register(t0);
Texture2D t_NormalGBuffer               : register(t1);
Texture2D t_RoughnessMetallicAOGBuffer  : register(t2);
Texture2D t_PositionGBuffer             : register(t3);
Texture2D t_SceneDepthGBuffer           : register(t4);

Texture2D t_ShadowDepth         : register(t10);

TextureCube tc_IrradianceMap    : register(t11);
TextureCube tc_EnvironmentMap   : register(t12);
Texture2D t_BrdfLUT             : register(t13);

Texture2D t_RayTracePassResult : register(t16);


// Samplers
// --------
sampler s_PointClampSampler : register(s0);
sampler s_LinearWrapSampler : register(s1);

// Function Signatures
// -------------------
void HDRToneMap(inout float3 target);
void GammaCorrect(inout float3 target);
float ShadowCalculation(float4 fragPosLightSpace, float3 normal, float3 lightDir);

// Pixel Shader Return Value
// -------------------------
struct PS_OUTPUT_LIGHTPASS
{
    float3 litImage : SV_Target;
};

// Entry Point
// -----------
PS_OUTPUT_LIGHTPASS main(PS_INPUT_LIGHTPASS ps_in)
{
    PS_OUTPUT_LIGHTPASS ps_out;
    
	// Sample Textures
    float3 albedo = pow(abs(t_AlbedoGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb), float3(2.2, 2.2, 2.2));
    float3 roughMetAOBufferSample = t_RoughnessMetallicAOGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    float3 worldPosition = t_PositionGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).xyz;
    float3 normal = t_NormalGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).xyz;
    float sceneDepth = t_SceneDepthGBuffer.Sample(s_LinearWrapSampler, ps_in.texCoords).r;
    float roughness = roughMetAOBufferSample.r;
    float metallic = roughMetAOBufferSample.g;
    float ambientOcclusion = roughMetAOBufferSample.b;
    
    float3 viewDirection = normalize(cbCameraPosition - worldPosition);
        
    float3 F0 = float3(0.04, 0.04, 0.04);
    float3 baseReflectivity = lerp(F0, albedo, metallic);
    float NdotV = max(dot(normal, viewDirection), 0.0000001);
    
    float3 spotLightLuminance = float3(0.0, 0.0, 0.0);
    float3 pointLightLuminance = float3(0.0, 0.0, 0.0);
    float3 directionalLightLuminance = float3(0.0, 0.0, 0.0);
    
    // Calculate Light Radiance
    // Directional Lights
    for (int d = 0; d < cbNumDirectionalLights; d++)
    {
        float3 lightDir = normalize(-dirLights.direction);
        
        // Shadowing
        float4 fragPosLightSpace = mul(float4(worldPosition, 1.0), mul(dirLights.lightSpaceView, dirLights.lightSpaceProj));
        float shadow;
        if (cbRayTraceEnabled)
        {
            shadow = t_RayTracePassResult.Sample(s_PointClampSampler, ps_in.texCoords).r;
        }
        else
        {
            shadow = ShadowCalculation(fragPosLightSpace, normal, lightDir);
        }
        
        directionalLightLuminance += CaclualteDirectionalLight(dirLights, viewDirection, normal, worldPosition, NdotV, albedo, roughness, metallic, baseReflectivity) * (shadow);
    }
    
    // Spot Lights
    for (int s = 0; s < cbNumSpotLights; s++)
    {
        spotLightLuminance += CalculateSpotLight(spotLights[s], viewDirection, NdotV, worldPosition, normal, albedo, roughness, metallic, baseReflectivity);
    }
    
    // Point Lights
    for (int p = 0; p < cbNumPointLights; p++)
    {
        pointLightLuminance += CalculatePointLight(pointLights[p], worldPosition, viewDirection, NdotV, normal, albedo, metallic, roughness, baseReflectivity);;
    }
    
    // IBL
    // Irradiance
    float3 F_IBL = FresnelSchlickRoughness(NdotV, baseReflectivity, roughness);
    float3 kD_IBL = (1.0f - F_IBL) * (1.0f - metallic);
    float3 diffuse_IBL = tc_IrradianceMap.Sample(s_LinearWrapSampler, normal).rgb * albedo * kD_IBL;

    // Specular IBL
    const float MAX_REFLECTION_MIP_LOD = 10.0f;
    float3 environmentMapColor = tc_EnvironmentMap.SampleLevel(s_LinearWrapSampler, reflect(-viewDirection, normal), roughness * MAX_REFLECTION_MIP_LOD).rgb;
    float2 brdf = t_BrdfLUT.Sample(s_LinearWrapSampler, float2(NdotV, roughness)).rg;
    float3 specular_IBL = environmentMapColor * (F_IBL * brdf.r + brdf.g);
    
    float3 ambient = (diffuse_IBL + specular_IBL) * ambientOcclusion;
    float3 combinedLightLuminance = (pointLightLuminance + spotLightLuminance) + (directionalLightLuminance);
    
     // Combine Light Luminance
    float3 pixelColor = ambient + combinedLightLuminance;
    
    HDRToneMap(pixelColor);
    GammaCorrect(pixelColor);
    
    ps_out.litImage.rgb = pixelColor;
    
    return ps_out;
}

void HDRToneMap(inout float3 target)
{
    target = float3(1.0, 1.0, 1.0) - exp(-target * cbCameraExposure);
}

void GammaCorrect(inout float3 target)
{
    const float gamma = 2.2;
    target = pow(abs(target.rgb), float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));
}

float ShadowCalculation(float4 fragPosLightSpace, float3 normal, float3 lightDir)
{
    float3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // Soften Shadows
    float shadow = 0.0;
    float2 texelSize = 1.0 / float2(1024.0, 1024.0);
    [unroll(2)]
    for (int x = -1; x <= 1; ++x)
    {
        [unroll(2)]
        for (int y = -1; y <= 1; ++y)
        {
            float depth = t_ShadowDepth.Sample(s_PointClampSampler, projCoords.xy + float2(x, y) * texelSize).r;
            shadow += (depth + bias) < projCoords.z ? 0.0 : 1.0;

        }
    }
    return (1.0 - shadow) / 9.0;
}
