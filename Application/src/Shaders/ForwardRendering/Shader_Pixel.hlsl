struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main(VS_OUTPUT ps_in) : SV_TARGET
{
    return ps_in.color;
}