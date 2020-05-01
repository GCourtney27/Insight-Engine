#pragma once
#include <DirectXMath.h>

// This is a structure of our contant buffers
struct CB_VS_PerObject
{
	DirectX::XMFLOAT4X4 wvpMatrix;
};

struct CB_PS_VS_PerFrame
{
	DirectX::XMFLOAT3 cameraPosition;
};

struct CB_PS_PointLight
{

};
