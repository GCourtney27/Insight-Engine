
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
    vs_out.sv_Position = float4(vs_in.Vertex, 1.0);
    vs_out.TexCoords = vs_in.TexCoord;
    
    return vs_out;
}
