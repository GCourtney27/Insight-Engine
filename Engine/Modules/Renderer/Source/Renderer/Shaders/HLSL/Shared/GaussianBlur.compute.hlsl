
RWTexture2D<float3> t_Ping : register(u0);
RWTexture2D<float3> t_Pong : register(u1);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
}
