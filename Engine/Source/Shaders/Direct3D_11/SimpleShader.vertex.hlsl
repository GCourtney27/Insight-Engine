
cbuffer cbPerFrame : register(b0)
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
    float3 Vertex : POSITION;
    float2 TexCoord : TEXCOORDS;
};

struct VS_OUT
{
    float4 sv_Position : SV_POSITION;
    float2 TexCoords : TEXCOORDS;
};

VS_OUT main(VS_IN vs_in)
{
    VS_OUT vs_out;
    vs_in.Vertex.y = sin(vs_in.Vertex.y * time);
    vs_out.sv_Position = float4(vs_in.Vertex, 1.0);
    vs_out.TexCoords = vs_in.TexCoord;
    
    return vs_out;
}
