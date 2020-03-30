struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main(VS_OUTPUT ps_in) : SV_TARGET
{
    //float depth = ps_in.position.z / ps_in.position.w;
    //return float4(depth, depth, depth, 1.0);
    return ps_in.color;
}