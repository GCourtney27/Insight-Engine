#pragma once
#include <DirectXMath.h>

struct CB_VS_PerObject
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

struct CB_PS_VS_PerFrame
{
	DirectX::XMFLOAT3 cameraPosition;
	DirectX::XMMATRIX invView;
	DirectX::XMMATRIX invProj;
	float cameraExposure;
	float cameraNearZ;
	float cameraFarZ;
	float deltaMs;
	float time;
	int numPointLights;
	int numDirectionalLights;
	int numSpotLights;
	float padding;
};

struct CB_PS_PointLight
{
	DirectX::XMFLOAT3 position;
	float strength;

	DirectX::XMFLOAT3 diffuse;
	float padding1;
};

struct CB_PS_DirectionalLight
{
	DirectX::XMFLOAT3 direction;
	float padding;

	DirectX::XMFLOAT3 diffuse;
	float strength;
};

struct CB_PS_SpotLight
{
	DirectX::XMFLOAT3 position;
	float innerCutOff;
	DirectX::XMFLOAT3 direction;
	float outerCutOff;

	DirectX::XMFLOAT3 diffuse;
	float strength;

};

struct CB_PS_PostFx
{
	// Vignette
	float innerRadius;
	float outerRadius;
	float opacity;
	int vnEnabled;

	// Film Grain
	float fgStrength;
	int fgEnabled;
};
