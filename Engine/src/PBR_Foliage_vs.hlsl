   #pragma pack_matrix(row_major)

// World View Projection(DirectX term) is the same thing as Model View Projection(OpenGL term)

cbuffer perObjectBuffer : register(b0) // Defined in ConstantBufferTypes
{
    matrix world;
    matrix view;
    matrix projection;
};

cbuffer PerFrame : register(b1)
{
    float deltaTime;
    float time;
}

cbuffer PerObjectUtil : register(b2) // Uploaded via material
{
    float2 uvOffset;
    float2 tiling;
    float3 vertOffset;
}

struct VS_INPUT // Defined in InitializeShaders() in TextuteredFoliageMaterial.cpp with D3D11_INPUT_ELEMENT_DESC
{
    float3 inPosition : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBiTangent : BITANGENT;
};

struct VS_OUTPUT // What this shader returns to the pixel shader with PS_INPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outTangent : TANGENT;
    float3 outBiTangent : BITANGENT;
    float3 outWorldPos : WORLD_POSITION;
};

float wave(float value, float frequency, float speed, float amplitude)
{
    return sin(value * frequency + speed) * amplitude;
}

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    matrix worldViewProj = mul(mul(world, view), projection);
    matrix worldView = mul(world, view);

    //output.outPosition = mul(float4(input.inPosition, 1.0f), worldViewProj); // WORKS - No Wind

    // Animation
    float4 objectSpacePos = float4(input.inPosition, 1.0f);
    objectSpacePos.x += wave(objectSpacePos.x, time * 0.3f, deltaTime * 1.0f, 1.0f);
    output.outPosition = mul(objectSpacePos, worldViewProj);

    output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), world));
    output.outTangent = mul(input.inTangent, (float3x3) worldView);
    output.outBiTangent = mul(input.inBiTangent, (float3x3) worldView);

    output.outWorldPos = mul(float4(input.inPosition, 1.0f), world);

	// Texture Scrolling
    output.outTexCoord = float2((input.inTexCoord.x + uvOffset.x) * tiling.x, (input.inTexCoord.y + uvOffset.y) * tiling.y);

    return output;
}