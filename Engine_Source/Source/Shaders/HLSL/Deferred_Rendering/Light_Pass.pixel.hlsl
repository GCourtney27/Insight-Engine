#include <../Common/Insight_Common.hlsli>
#include <Deferred_Rendering.hlsli>	

#define SHADOW_DEPTH_BIAS 0.00005f

// G-Buffer Inputs
// --------------
Texture2D t_AlbedoGBuffer                       : register(t0);
Texture2D t_NormalGBuffer                       : register(t1);
Texture2D t_RoughnessMetallicAOSpecularGBuffer  : register(t2);
Texture2D t_PositionGBuffer                     : register(t3);
Texture2D t_SceneDepthGBuffer                   : register(t4);

Texture2D t_ShadowDepth         : register(t10);

TextureCube tc_IrradianceMap    : register(t11);
TextureCube tc_RadianceMap      : register(t12);
Texture2D t_BrdfLUT             : register(t13);

Texture2D t_RayTracePassResult : register(t16);


// Samplers
// --------
sampler s_PointBorderSampler : register(s0);
sampler s_LinearWrapSampler : register(s1);
sampler s_LinearClampSampler : register(s2);

// Function Signatures
// -------------------
void HDRToneMap(inout float3 target);
void GammaCorrect(inout float3 target);
float ShadowCalculation(float4 fragPosLightSpace, float3 WorldNormal, float3 LightDir);

// Pixel Shader Return Value
// -------------------------
struct PS_OUTPUT_LIGHTPASS
{
    float4 LitImage     : SV_Target0;
    float3 BloomBuffer  : SV_Target1;
};

void LinearizeDepth(inout float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    depth = (2.0 * cbCameraNearZ * cbCameraFarZ) / (cbCameraFarZ + cbCameraNearZ - z * (cbCameraFarZ - cbCameraNearZ)) / cbCameraFarZ;
}
// Entry Point
// -----------
PS_OUTPUT_LIGHTPASS main(PS_INPUT_LIGHTPASS ps_in)
{
    PS_OUTPUT_LIGHTPASS ps_out;
    ps_out.BloomBuffer  = float3(0.0f, 0.0f, 0.0f);
    ps_out.LitImage     = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
	// Sample Textures
    float2 PixelCoords = ps_in.texCoords * cbScreenSize;
    int3 LoadLocation = int3(PixelCoords, 0.0);
    
    float3 WorldNormal = t_NormalGBuffer.Load(LoadLocation).rgb;
    float3 WorldPosition = t_PositionGBuffer.Load(LoadLocation).rgb;
    float3 Albedo = pow(t_AlbedoGBuffer.Load(LoadLocation).rgb, float3(2.2, 2.2, 2.2));
    float4 roughMetAOSpecGBuffSample = t_RoughnessMetallicAOSpecularGBuffer.Load(LoadLocation);
    
    float Roughness = roughMetAOSpecGBuffSample.r;
    float Metallic = roughMetAOSpecGBuffSample.g;
    float StaticAO = roughMetAOSpecGBuffSample.b;
    float Specular = roughMetAOSpecGBuffSample.a;
        
    float3 ViewDirection = normalize(cbCameraPosition - WorldPosition);
        
    float3 F0 = float3(Specular, Specular, Specular);
    float3 baseReflectivity = lerp(F0, Albedo, Metallic);
    float NdotV = max(dot(WorldNormal, ViewDirection), 0.0000001);
    
    float3 spotLightLuminance = float3(0.0, 0.0, 0.0);
    float3 pointLightLuminance = float3(0.0, 0.0, 0.0);
    float3 directionalLightLuminance = float3(0.0, 0.0, 0.0);
    
    // Calculate Light Radiance
    // Directional Lights
    for (int d = 0; d < cbNumDirectionalLights; d++)
    {
        float3 LightDir = normalize(-dirLight.direction);
        
        // Shadowing
        float shadow = 1.0f;
        if (cbRayTraceEnabled)
        {
            shadow = t_RayTracePassResult.Sample(s_PointBorderSampler, ps_in.texCoords).r;
        }
        else
        {
            float4 fragPosLightSpace = mul(mul(float4(WorldPosition, 1.0), dirLight.lightSpaceView), dirLight.lightSpaceProj);
            shadow = ShadowCalculation(fragPosLightSpace, WorldNormal, LightDir);

            // Visuaize RT shadow map
            //ps_out.LitImage = float4(shadow, shadow, shadow, 1.0);
            //return ps_out;
        }
        
        directionalLightLuminance += CaclualteDirectionalLight(dirLight, ViewDirection, WorldNormal, WorldPosition, NdotV, Albedo, Roughness, Metallic, baseReflectivity) * (shadow);
    }
    
    // Spot Lights
    for (int s = 0; s < cbNumSpotLights; s++)
    {
        spotLightLuminance += CalculateSpotLight(spotLights[s], ViewDirection, NdotV, WorldPosition, WorldNormal, Albedo, Roughness, Metallic, baseReflectivity);
    }
    
    // Point Lights
    for (int p = 0; p < cbNumPointLights; p++)
    {
        pointLightLuminance += CalculatePointLight(pointLights[p], WorldPosition, ViewDirection, NdotV, WorldNormal, Albedo, Metallic, Roughness, baseReflectivity);;
    }
    
    // IBL
    // Irradiance
    float3 F_IBL = FresnelSchlickRoughness(max(NdotV, 0.0), baseReflectivity, Roughness);
    float3 kD_IBL = (1.0f - F_IBL) * (1.0f - Metallic);
    float3 diffuse_IBL = tc_IrradianceMap.Sample(s_LinearClampSampler, WorldNormal).rgb * Albedo * kD_IBL;

    // Specular IBL
    const float MAX_REFLECTION_MIP_LOD = 7.0f;
    float3 environmentMapColor = tc_RadianceMap.SampleLevel(s_LinearClampSampler, reflect(-ViewDirection, WorldNormal), Roughness * MAX_REFLECTION_MIP_LOD).rgb;
    float2 brdf = t_BrdfLUT.Sample(s_LinearClampSampler, float2(NdotV, Roughness)).rg;
    float3 Specular_IBL = environmentMapColor * (F_IBL * brdf.r + brdf.g);
    
    float3 ambient = (diffuse_IBL + Specular_IBL) * StaticAO;
    float3 combinedLightLuminance = (pointLightLuminance + spotLightLuminance) + (directionalLightLuminance);
    
     // Combine Light Luminance
    float3 pixelColor = ambient + combinedLightLuminance;
    
    // Bloom
    float brightness = dot(pixelColor.rgb, float3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
        ps_out.BloomBuffer.rgb = pixelColor;
    else
        ps_out.BloomBuffer.rgb = float3(0.0, 0.0, 0.0);
    
    // Color Correction
    //HDRToneMap(pixelColor);
    //GammaCorrect(pixelColor);
    ps_out.LitImage.rgb = pixelColor;
    
    return ps_out;
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

float ShadowCalculation(float4 fragPosLightSpace, float3 WorldNormal, float3 LightDir)
{
    float3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if (projCoords.z > 1.0)
    {
        projCoords.z = 1.0;
    }
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(WorldNormal, LightDir)), 0.005);
    
    // Soften Shadows
    uint3 texDimensions = int3(0, 0, 0);
    t_ShadowDepth.GetDimensions(0, texDimensions.x, texDimensions.y, texDimensions.z);
    float shadow = 0.0;
    float2 texelSize = 1.0 / texDimensions.xy;
    [unroll(2)]
    for (int x = -1; x <= 1; ++x)
    {
        [unroll(2)]
        for (int y = -1; y <= 1; ++y)
        {
            float depth = t_ShadowDepth.Sample(s_PointBorderSampler, projCoords.xy + float2(x, y) * texelSize).r;
            shadow += (currentDepth - bias) > depth ? 1.0 : 0.0;

        }
    }
    return shadow /= 9.0;
}
