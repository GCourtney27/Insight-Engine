   #pragma pack_matrix(row_major)

// World View Projection(DirectX term) is the same thing as Model View Projection(OpenGL term)

cbuffer perObjectBuffer : register(b0) // Defined in ConstantBufferTypes
{
    matrix world;
    matrix view;
    matrix projection;
};

cbuffer PerFrame : register(b1) // Uploaded via Graphics.cpp
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

cbuffer Wave : register(b3) // Uploaded via material
{
    float frequency;
    float amplitude;
    float speed;
}

struct VS_INPUT // Defined in InitializeShaders() with D3D11_INPUT_ELEMENT_DESC
{
    float3 inPosition : POSITION;
    float2 inTexCoord0 : TEXCOORD;
    float2 inTexCoord1 : TEXCOORDNEW;
    float3 inNormal : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBiTangent : BITANGENT;
};

struct VS_OUTPUT // What this shader returns to the pixel shader with PS_INPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord0 : TEXCOORD;
    float2 outTexCoord1 : TEXCOORDNEW;
    float3 outNormal : NORMAL;
    float3 outTangent : TANGENT;
    float3 outBiTangent : BITANGENT;
    float3 outWorldPos : WORLD_POSITION;
};

float wave(float value, float frequency, float horizontalOffset, float amplitude)
{
    return sin(frequency * (value + horizontalOffset)) * amplitude;
}

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    matrix worldViewProj = mul(mul(world, view), projection);
    matrix worldView = mul(world, view);
    

    // Animation
    float4 objectSpacePos = float4(input.inPosition, 1.0f);

    objectSpacePos.y += wave(objectSpacePos.x, 0.50f, time, 0.01f);
                                                     
    objectSpacePos.y += wave(objectSpacePos.z, 0.50f, time, 0.01f);
    


    output.outPosition = mul(objectSpacePos, worldViewProj);

    output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), world));
    output.outTangent = mul(input.inTangent, (float3x3) worldView);
    output.outBiTangent = mul(input.inBiTangent, (float3x3) worldView);

    output.outWorldPos = mul(float4(input.inPosition, 1.0f), world);

	// Texture Scrolling
    output.outTexCoord0 = float2((input.inTexCoord0.x + (uvOffset.x + (time * 0.01f))) * tiling.x, (input.inTexCoord0.y + (uvOffset.y + (time * -0.01f))) * tiling.y);
    output.outTexCoord1 = float2(-(input.inTexCoord1.x + (uvOffset.x + (time * 0.01f))) * tiling.x, (input.inTexCoord1.y + (uvOffset.y + (time * 0.01f))) * tiling.y);

    return output;
}