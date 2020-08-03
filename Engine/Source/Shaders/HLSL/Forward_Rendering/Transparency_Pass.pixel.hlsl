#include <Forward_Rendering.hlsli>

Texture2D t_Albedo      : register(t0);
Texture2D t_Normal      : register(t1);
Texture2D t_Roughness   : register(t2);
Texture2D t_Opacity     : register(t3);
Texture2D t_Translucency     : register(t4);

Texture2D t_ShadowDepth         : register(t5);
Texture2D t_SceneDepth         : register(t6);

TextureCube tc_IrradianceMap : register(t7);
TextureCube tc_EnvironmentMap : register(t8);
Texture2D t_BrdfLUT : register(t9);

// Samplers
// --------
sampler s_PointClampSampler : register(s0);
SamplerState s_LinearWrapSampler : register(s1);

// Function Signatures
// -------------------
void HDRToneMap(inout float3 target);
void GammaCorrect(inout float3 target);
float ShadowCalculation(float4 fragPosLightSpace, float3 normal, float3 lightDir);


struct PS_INPUT
{
    float4 sv_position : SV_POSITION;
    float3 FragPos : FRAG_POS;
    float2 texCoords : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
};

struct PS_OUT
{
    float4 LitImage : SV_TARGET;
};

// Function Signatures
// -------------------
float3 CalculatePointLight(PointLight light, float3 normal, float3 fragPosition, float3 viewDirection, float2 texCoords);

PS_OUT main(PS_INPUT ps_in)
{
    PS_OUT ps_out;
    ps_out.LitImage = float4(1.0, 0.0, 0.0, 1.0);
    return ps_out;
    // Sample Textures
    float3 albedo = pow(abs(t_Albedo.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb), float3(2.2, 2.2, 2.2)) + diffuseAdditive;
    float3 normal = t_Normal.Sample(s_LinearWrapSampler, ps_in.texCoords).xyz;
    float roughnessInput = t_Roughness.Sample(s_LinearWrapSampler, ps_in.texCoords).r;
    float opacity = t_Opacity.Sample(s_LinearWrapSampler, ps_in.texCoords).r;
    float3 translucency = t_Translucency.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    float roughness = roughnessInput + roughnessAdditive;
    
    float3 viewDirection = normalize(cameraPosition - ps_in.FragPos);
        
    float3 F0 = float3(0.04, 0.04, 0.04);
    float3 baseReflectivity = lerp(F0, albedo, 0.0);
    float NdotV = max(dot(normal, viewDirection), 0.0000001);
    
    float3 spotLightLuminance = float3(0.0, 0.0, 0.0);
    float3 pointLightLuminance = float3(0.0, 0.0, 0.0);
    float3 directionalLightLuminance = float3(0.0, 0.0, 0.0);
    
    // Calculate Light Radiance
    // Directional Lights
    for (int d = 0; d < numDirectionalLights; d++)
    {
        float3 lightDir = normalize(-dirLights[d].direction);
        
        // Shadowing
        float4 fragPosLightSpace = mul(float4(ps_in.FragPos, 1.0), mul(dirLights[d].lightSpaceView, dirLights[d].lightSpaceProj));
        float shadow = ShadowCalculation(fragPosLightSpace, normal, lightDir);
        
        directionalLightLuminance += CaclualteDirectionalLight(dirLights[d], viewDirection, normal, ps_in.FragPos, NdotV, albedo, roughness, metallicAdditive, baseReflectivity) * (1.0 - shadow);
    }
    
    // Spot Lights
    for (int s = 0; s < numPointLights; s++)
    {
        spotLightLuminance += CalculateSpotLight(spotLights[s], viewDirection, NdotV, ps_in.FragPos, normal, albedo, roughness, metallicAdditive, baseReflectivity);
    }
    
    // Point Lights
    for (int p = 0; p < numPointLights; p++)
    {
        pointLightLuminance += CalculatePointLight(pointLights[p], ps_in.FragPos, viewDirection, NdotV, normal, albedo, metallicAdditive, roughness, baseReflectivity);;
    }
    
    // IBL
    // Irradiance
    float3 F_IBL = FresnelSchlickRoughness(NdotV, baseReflectivity, roughness);
    float3 kD_IBL = (1.0f - F_IBL) * (1.0f - metallicAdditive);
    float3 diffuse_IBL = tc_IrradianceMap.Sample(s_LinearWrapSampler, normal).rgb * albedo * kD_IBL;

    // Specular IBL
    const float MAX_REFLECTION_MIP_LOD = 10.0f;
    float3 environmentMapColor = tc_EnvironmentMap.SampleLevel(s_LinearWrapSampler, reflect(-viewDirection, normal), roughness * MAX_REFLECTION_MIP_LOD).rgb;
    float2 brdf = t_BrdfLUT.Sample(s_LinearWrapSampler, float2(NdotV, roughness)).rg;
    float3 specular_IBL = environmentMapColor * (F_IBL * brdf.r + brdf.g);
    
    float3 ambient = (diffuse_IBL + specular_IBL);
    float3 combinedLightLuminance = (pointLightLuminance + spotLightLuminance) + (directionalLightLuminance);
    
     // Combine Light Luminance
    float3 pixelColor = ambient + combinedLightLuminance;
    
    HDRToneMap(pixelColor);
    GammaCorrect(pixelColor);
    
    ps_out.LitImage = float4(pixelColor, opacity);
    
    return ps_out;
}

void HDRToneMap(inout float3 target)
{
    target = float3(1.0, 1.0, 1.0) - exp(-target * cameraExposure);
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
