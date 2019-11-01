#pragma once
#include <DirectXMath.h>

// ConstantBufferTypes are owned by ConstantBuffer obejcts

struct CB_VS_vertexshader
{
	// Named perObjectBuffer in vertexshader.hlsl
	DirectX::XMMATRIX worldMatrix;//16
	DirectX::XMMATRIX viewMatrix;//16
	DirectX::XMMATRIX projectionMatrix;//16

};

struct CB_PS_directionalLight
{
	DirectX::XMFLOAT3 Color;//12
	float Strength;//4
	//16
	DirectX::XMFLOAT3 Direction;//12
	float padding;
	//16
};

struct CB_PS_light
{
	DirectX::XMFLOAT3 ambientLightColor;//12
	float ambientLightStrength;//4
	//16
	DirectX::XMFLOAT3 dynamicLightColor;//12
	float dynamicLightStrength;//4
	//16
	DirectX::XMFLOAT3 dynamicLightPosition;//12
	float dynamicLightAttenuation_a;//4
	//16
	float dynamicLightAttenuation_b;//4
	float dynamicLightAttenuation_c;//4
	float padding[3];
	//16
};

struct CB_VS_vertexshader_2d
{
	DirectX::XMMATRIX wvpMatrix;//16
	//16
};

struct CB_PS_perframe
{
	DirectX::XMFLOAT3 camPosition;//12
	float deltaTime;//4
	//16
};

struct CB_VS_perframe
{
	float deltaTime;//4
	float padding[3];//12
	//16
	DirectX::XMFLOAT2 uvOffset;//8
	DirectX::XMFLOAT3 vertOffset;//8
	//16
};

struct CB_PS_PerObjectColor
{
	DirectX::XMFLOAT3 color;//16
	//16
	float metallic;//4
	float roughnss;//4
	float padding[2];//8
	//16
};

struct CB_VS_Sky
{
	DirectX::XMMATRIX wvpMatrix;//16
	DirectX::XMMATRIX worldMatrix;//16
};