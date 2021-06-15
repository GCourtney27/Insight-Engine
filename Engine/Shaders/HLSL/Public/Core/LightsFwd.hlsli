
//
// Common Light Stuctures.
//

struct PointLight
{
    float4 Position;
    float4 Color;
    float Brightness;
    uint Id;
};

struct DirectionalLight
{
    float4 Direction;
    float4 Color;
    float Brightness;
    uint Id;
};

