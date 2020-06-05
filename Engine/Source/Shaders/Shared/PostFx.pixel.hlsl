#include <../DeferredRendering/Deferred_Rendering.hlsli>
//#define mod(x, y) (x - y * floor(x / y))
//#define Scale float3(0.8, 0.8, 0.8)
//#define K 19.19

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

bool traceScreenSpaceRay(
    float3 csOrig,
    float3 csDir,
    float jitter,
    out float2 hitPixel,
    out float3 hitPoint);
float distanceSquared(float2 a, float2 b);
bool intersectsDepthBuffer(float z, float minZ, float maxZ);
void swap(inout float a, inout float b);
float linearDepthTexelFetch(int2 hitPixel);



struct PS_INPUT_POSTFX
{
    float4 sv_position : SV_POSITION;
    float2 texCoords : TEXCOORD;
};

float4 main(PS_INPUT_POSTFX ps_in) : SV_TARGET
{
    int3 loadIndices = int3(ps_in.sv_position.xy, 0);
    float3 normalVS = t_NormalGBuffer.Load(loadIndices).xyz;
    if (!any(normalVS))
    {
        return 0.0f;
    }

    float depth = t_DepthGBuffer.Load(loadIndices).r;
    float3 rayOriginVS = viewRay * LinearizeDepth(depth);

    /*
     * Since position is reconstructed in view space, just normalize it to get the
     * vector from the eye to the position and then reflect that around the normal to
     * get the ray direction to trace.
     */
    float3 toPositionVS = normalize(rayOriginVS);
    float3 rayDirectionVS = normalize(reflect(toPositionVS, normalVS));

    // output rDotV to the alpha channel for use in determining how much to fade the ray
    float rDotV = dot(rayDirectionVS, toPositionVS);

    // out parameters
    float2 hitPixel = float2(0.0f, 0.0f);
    float3 hitPoint = float3(0.0f, 0.0f, 0.0f);

    float jitter = stride > 1.0f ? float(int(ps_in.sv_position.x + ps_in.sv_position.y) & 1) * 0.5f : 0.0f;

    // perform ray tracing - true if hit found, false otherwise
    bool intersection = traceScreenSpaceRay(rayOriginVS, rayDirectionVS, jitter, hitPixel, hitPoint);

    return t_LightPassResult.Sample(s_LinearWrapSampler, hitPoint.xy);
    
    depth = t_DepthGBuffer.Load(int3(hitPixel, 0)).r;

    // move hit pixel from pixel position to UVs
    hitPixel *= float2(texelWidth, texelHeight);
    if (hitPixel.x > 1.0f || hitPixel.x < 0.0f || hitPixel.y > 1.0f || hitPixel.y < 0.0f)
    {
        intersection = false;
    }

    return float4(hitPixel, depth, rDotV) * (intersection ? 1.0f : 0.0f);
    
    //float3 result = t_LightPassResult.Sample(s_LinearWrapSampler, ps_in.texCoords).rgb;
    //if (vnEnabled)
    //{
    //    result = AddVignette(result, ps_in.texCoords);
    //}
    //if (fgEnabled)
    //{
    //    result = AddFilmGrain(result, ps_in.texCoords);
    //}
    
    //HDRToneMap(result);
    //GammaCorrect(result);
    //return float4(result, 1.0);
}

float mod(float x, float y)
{
    return (x - y * floor(x / y));
}

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
    float3 color = float3(1.0, 1.0, 1.0);

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
    target = pow(abs(target.rgb), float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * cameraNearZ * cameraFarZ) / (cameraFarZ + cameraNearZ - z * (cameraFarZ - cameraNearZ)) / cameraFarZ;
}

float distanceSquared(float2 a, float2 b)
{
    a -= b;
    return dot(a, a);
}

bool intersectsDepthBuffer(float z, float minZ, float maxZ)
{
    /*
     * Based on how far away from the camera the depth is,
     * adding a bit of extra thickness can help improve some
     * artifacts. Driving this value up too high can cause
     * artifacts of its own.
     */
    float depthScale = min(1.0f, z * strideZCutoff);
    z += zThickness + lerp(0.0f, 2.0f, depthScale);
    return (maxZ >= z) && (minZ - zThickness <= z);
}

void swap(inout float a, inout float b)
{
    float t = a;
    a = b;
    b = t;
}

float linearDepthTexelFetch(int2 hitPixel)
{
    // Load returns 0 for any value accessed out of bounds
    return LinearizeDepth(t_DepthGBuffer.Load(int3(hitPixel, 0)).r);
}

// Returns true if the ray hit something
bool traceScreenSpaceRay(
    // Camera-space ray origin, which must be within the view volume
    float3 csOrig,
    // Unit length camera-space ray direction
    float3 csDir,
    // Number between 0 and 1 for how far to bump the ray in stride units
    // to conceal banding artifacts. Not needed if stride == 1.
    float jitter,
    // Pixel coordinates of the first intersection with the scene
    out float2 hitPixel,
    // Camera space location of the ray hit
    out float3 hitPoint)
{
    // Clip to the near plane
    float rayLength = ((csOrig.z + csDir.z * maxDistance) < cameraNearZ) ?
    (cameraNearZ - csOrig.z) / csDir.z : maxDistance;
    float3 csEndPoint = csOrig + csDir * rayLength;

    // Project into homogeneous clip space
    float4 H0 = mul(float4(csOrig, 1.0f), invView);
    H0.xy *= depthBufferSize;
    float4 H1 = mul(float4(csEndPoint, 1.0f), invView);
    H1.xy *= depthBufferSize;
    float k0 = 1.0f / H0.w;
    float k1 = 1.0f / H1.w;

    // The interpolated homogeneous version of the camera-space points
    float3 Q0 = csOrig * k0;
    float3 Q1 = csEndPoint * k1;

    // Screen-space endpoints
    float2 P0 = H0.xy * k0;
    float2 P1 = H1.xy * k1;

    // If the line is degenerate, make it cover at least one pixel
    // to avoid handling zero-pixel extent as a special case later
    P1 += (distanceSquared(P0, P1) < 0.0001f) ? float2(0.01f, 0.01f) : 0.0f;
    float2 delta = P1 - P0;

    // Permute so that the primary iteration is in x to collapse
    // all quadrant-specific DDA cases later
    bool permute = false;
    if (abs(delta.x) < abs(delta.y))
    {
        // This is a more-vertical line
        permute = true;
        delta = delta.yx;
        P0 = P0.yx;
        P1 = P1.yx;
    }

    float stepDir = sign(delta.x);
    float invdx = stepDir / delta.x;

    // Track the derivatives of Q and k
    float3 dQ = (Q1 - Q0) * invdx;
    float dk = (k1 - k0) * invdx;
    float2 dP = float2(stepDir, delta.y * invdx);

    // Scale derivatives by the desired pixel stride and then
    // offset the starting values by the jitter fraction
    float strideScale = 1.0f - min(1.0f, csOrig.z * strideZCutoff);
    float pxStride = 1.0f + strideScale * stride;
    dP *= pxStride;
    dQ *= pxStride;
    dk *= pxStride;

    P0 += dP * jitter;
    Q0 += dQ * jitter;
    k0 += dk * jitter;

    // Slide P from P0 to P1, (now-homogeneous) Q from Q0 to Q1, k from k0 to k1
    float4 PQk = float4(P0, Q0.z, k0);
    float4 dPQk = float4(dP, dQ.z, dk);
    float3 Q = Q0;

    // Adjust end condition for iteration direction
    float end = P1.x * stepDir;

    float stepCount = 0.0f;
    float prevZMaxEstimate = csOrig.z;
    float rayZMin = prevZMaxEstimate;
    float rayZMax = prevZMaxEstimate;
    float sceneZMax = rayZMax + 100.0f;
    for (;
        ((PQk.x * stepDir) <= end) && (stepCount < maxSteps) &&
        !intersectsDepthBuffer(sceneZMax, rayZMin, rayZMax) &&
        (sceneZMax != 0.0f);
        ++stepCount)
    {
        rayZMin = prevZMaxEstimate;
        rayZMax = (dPQk.z * 0.5f + PQk.z) / (dPQk.w * 0.5f + PQk.w);
        prevZMaxEstimate = rayZMax;
        if (rayZMin > rayZMax)
        {
            swap(rayZMin, rayZMax);
        }

        hitPixel = permute ? PQk.yx : PQk.xy;
        // You may need hitPixel.y = depthBufferSize.y - hitPixel.y; here if your vertical axis
        // is different than ours in screen space
        sceneZMax = linearDepthTexelFetch(int2(hitPixel));

        PQk += dPQk;
    }

    // Advance Q based on the number of steps
    Q.xy += dQ.xy * stepCount;
    hitPoint = Q * (1.0f / PQk.w);
    return intersectsDepthBuffer(sceneZMax, rayZMin, rayZMax);
}