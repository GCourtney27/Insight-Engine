#pragma once
#include <DirectXMath.h>

struct CB_VS_PerObject
{
	DirectX::XMFLOAT4X4 world;
};

struct CB_PS_VS_PerObjectAdditives
{
	float roughnessAdditive;
	float metallicAdditive;
	DirectX::XMFLOAT2 uvOffset;

	DirectX::XMFLOAT2 tiling;
	float padding1;
	float padding2;
	
	DirectX::XMFLOAT3 diffuseAdditive;
	float padding3;
};

struct CB_PS_VS_PerFrame
{
	DirectX::XMFLOAT3 cameraPosition;
	float cameraExposure;//4
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	float cameraNearZ;
	float cameraFarZ;
	float deltaMs;
	float time;//4
	float numPointLights;
	float numDirectionalLights;
	float numSpotLights;
	DirectX::XMFLOAT2 screenSize;//4

	float padding;
	float padding1;
	float padding2;
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
	float innerCutoff;
	DirectX::XMFLOAT3 direction;
	float outerCutoff;

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

	// Chromatic Aberration
	int caEnabled;
	float caIntensity;
};