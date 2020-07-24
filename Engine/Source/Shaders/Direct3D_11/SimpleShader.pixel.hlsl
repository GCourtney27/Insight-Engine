
struct PS_IN
{
    float4 sv_position : SV_POSITION;
    float2 TexCoords : TEXCOORDS;
};

float4 main(PS_IN ps_in) : SV_TARGET
{
    return float4(ps_in.TexCoords, 0.0f, 1.0f);
}