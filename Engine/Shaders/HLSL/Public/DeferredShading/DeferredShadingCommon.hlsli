
//
// Geometry Pass In/Out Structures
//

// Vertex Stage
struct GP_VSInput
{
	float3 Position		: POSITION;
	float3 Normal		: NORMAL;
	float3 Tangent		: TANGENT;
	float3 BiTangent	: BITANGENT;
	float4 Color		: COLOR;
	float2 UVs			: UVS;
};
struct GP_PSInput
{
	float4 Position		: SV_POSITION;
	float3 WorldPos		: WORLDPOS;
	float3 Normal		: NORMAL;
	float3 Tangent		: TANGENT;
	float3 BiTangent	: BITANGENT;
	float4 VertexColor	: COLOR;
	float2 UVs			: UVS;
};

// Pixel Stage
struct GP_PSOutput
{
	float4 Albedo	: SV_Target0;
	float4 Normal	: SV_Target1;
	float4 Position : SV_Target2;
};


// 
// Light Pass In/Out Structures
//

// Vertex Stage
struct LP_VSInput
{
	float2 Position : POSITION;
	float2 UVs		: UVS;
};
struct LP_PSInput
{
	float4 Position : SV_POSITION;
	float2 UVs		: UVS;
};

// Pixel Stage
struct LP_PSOutput
{
	float4 Result : SV_Target;
};
