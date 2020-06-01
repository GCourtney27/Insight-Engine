#pragma once
#include <DirectXMath.h>

// This is a structure of our contant buffers
struct CB_VS_PerObject
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

struct CB_PS_VS_PerFrame
{
	DirectX::XMFLOAT3 cameraPosition;
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
	float padding;

	DirectX::XMFLOAT3 diffuse;
	float padding1;
};

struct CB_PS_DirectionalLight
{
	DirectX::XMFLOAT3 direction;

	DirectX::XMFLOAT3 ambient;
	DirectX::XMFLOAT3 diffuse;
	DirectX::XMFLOAT3 specular;
};

struct CB_PS_SpotLight
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;
	float innerCutOff;
	float outerCutOff;

	DirectX::XMFLOAT3 ambient;
	DirectX::XMFLOAT3 diffuse;
	DirectX::XMFLOAT3 specular;

	float constantFactor;
	float linearFactor;
	float quadraticFactor;

	float padding;
};
