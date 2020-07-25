
cbuffer cbPerObject : register(b0)
{
    float4x4 world;
};

cbuffer cbPerFrame : register(b1)
{
    float3 cameraPosition;
    float cameraExposure; //4
    float4x4 view;
    float4x4 projection;
    float cameraNearZ;
    float cameraFarZ;
    float deltaMs;
    float time; //4
    float numPointLights;
    float numDirectionalLights;
    float numSpotLights;
    float padding; //4
    float2 screenSize;
    float padding1;
    float padding2;
};

struct VS_IN
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORDS;
};

struct VS_OUT
{
    float4 sv_Position : SV_POSITION;
    float2 TexCoords : TEXCOORDS;
    float3 FragPos : FRAGPOS;
};

VS_OUT main(VS_IN vs_in)
{
    VS_OUT vs_out;
    
    matrix worldView = mul(world, view);
    float4x4 worldViewProjection = mul(mul(world, view), projection);
    float4 worldPos = mul(float4(vs_in.Position, 1.0), world);
    
    vs_out.sv_Position = mul(float4(vs_in.Position, 1.0f), worldViewProjection);
	
    vs_out.FragPos = worldPos.xyz;
    
    //vs_in.Position.y = sin(vs_in.Position.y * time);
    //vs_out.sv_Position = float4(vs_in.Position, 1.0);
    vs_out.TexCoords = vs_in.TexCoord;
    
    return vs_out;
}
