
struct VS_IN
{
    float3 Vertex : POSITION;
    float2 TexCoord : TEXCOORDS;
};

float4 main(VS_IN vs_in) : SV_POSITION
{
    return float4(vs_in.Vertex, 1.0);
}
