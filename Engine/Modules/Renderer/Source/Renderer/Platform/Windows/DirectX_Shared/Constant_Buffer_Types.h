#pragma once

#include <Insight/Core.h>

#include <DirectXMath.h>

/*
	** Warning: These buffers are 16 byte aligned, edit with caution **

	Constant buffers to be sent to the GPU during draw calls.
	Naming convention is as follows:
	CB_ShaderVisibility_NameOfConstantBuffer
	
	Ex) CB_PS_VS_PerObjectAdditives - Is a Constant Buffer (CB) visible to the Vertex Shader (VS) and Pixel Shader (PS) with a name of PerObjectAdditives

*/

struct CB_VS_PerObject
{
	DirectX::XMMATRIX World;
};

struct CB_PS_VS_PerObjectMaterialAdditives
{
	float RoughnessAdditive;
	float MetallicAdditive;
	DirectX::XMFLOAT2 UVOffset;

	DirectX::XMFLOAT2 UVTiling;
	float Padding1;
	float Padding2;
	
	DirectX::XMFLOAT3 DiffuseAdditive;
	float Padding3;
};

struct CB_PS_VS_PerFrame
{
	DirectX::XMFLOAT3 CameraPosition;
	float CameraExposure;//4
	DirectX::XMMATRIX View;//4x4
	DirectX::XMMATRIX InverseView;//4x4
	DirectX::XMMATRIX Projection;//4x4
	DirectX::XMMATRIX InverseProjection;//4x4
	float CameraNearZ;
	float CameraFarZ;
	float DeltaMs;
	float WorldTime;//4
	float NumPointLights;
	float NumDirectionalLights;
	float NumSpotLights;
	float RayTraceEnabled;
	DirectX::XMFLOAT2 ScreenSize;
	float Padding1;
	float Padding2;

};

struct CB_PS_PointLight
{
	DirectX::XMFLOAT3 Position;
	float Strength;//4

	DirectX::XMFLOAT3 DiffuseColor;
	float Padding1;//4
};

struct CB_PS_DirectionalLight
{
	DirectX::XMFLOAT3 Direction;
	float ShadowDarknessMultiplier;//4

	DirectX::XMFLOAT3 DiffuseColor;
	float Strength;//4

	DirectX::XMFLOAT4X4 LightSpaceView;
	DirectX::XMFLOAT4X4 LightSpaceProj;
};

struct CB_PS_SpotLight
{
	DirectX::XMFLOAT3 Position;
	float InnerCutoff;//4
	DirectX::XMFLOAT3 Direction;
	float OuterCutoff;//4

	DirectX::XMFLOAT3 DiffuseColor;
	float Strength;//4
};

struct CB_PS_PostFx
{
	// Vignette
	float vnInnerRadius;
	float vnOuterRadius;
	float vnOpacity;
	int vnEnabled = 0;

	// Film Grain
	float fgStrength;
	int fgEnabled = 0;

	// Chromatic Aberration
	int caEnabled = 0;
	float caIntensity;
};

struct CB_PS_Lights
{
	CB_PS_PointLight PointLights[MAX_POINT_LIGHTS_SUPPORTED];
	CB_PS_DirectionalLight DirectionalLight;
	CB_PS_SpotLight SpotLights[MAX_SPOT_LIGHTS_SUPPORTED];
};

struct CB_CS_DownSampleParams
{
	alignas(16) float Threshold; // Auto align as 16 bytes 
};
