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
	float deltaMs;
	float time;
};

struct CB_PS_PointLight
{

};
