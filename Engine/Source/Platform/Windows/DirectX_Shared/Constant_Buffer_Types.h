#pragma once

#include <Insight/Core.h>

#include <DirectXMath.h>

/*
	** These buffers are 16 byte aligned, edit with caution **

	Constant buffers to be sent to the GPU during draw calls.
	Naming convention is as follows:
	CB_ShaderVisibility_NameOfConstantBuffer
	
	Ex) CB_PS_VS_PerObjectAdditives - Is a Constant Buffer (CB) visible to the Vertex Shader (VS) and Pixel Shader (PS) with a name of PerObjectAdditives

*/

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
	DirectX::XMFLOAT4X4 view;//4x4
	DirectX::XMFLOAT4X4 inverseView;//4x4
	DirectX::XMFLOAT4X4 projection;//4x4
	DirectX::XMFLOAT4X4 inverseProjection;//4x4
	float cameraNearZ;
	float cameraFarZ;
	float DeltaMs;
	float time;//4
	float numPointLights;
	float numDirectionalLights;
	float numSpotLights;
	float padding;
	DirectX::XMFLOAT2 screenSize;
	float padding1;
	float padding2;

};

struct CB_PS_PointLight
{
	DirectX::XMFLOAT3 position;
	float strength;//4

	DirectX::XMFLOAT3 diffuse;
	float padding1;//4
};

struct CB_PS_DirectionalLight
{
	DirectX::XMFLOAT3 direction;
	float padding;//4

	DirectX::XMFLOAT3 diffuse;
	float strength;//4

	DirectX::XMFLOAT4X4 lightSpaceView;
	DirectX::XMFLOAT4X4 lightSpaceProj;
};

struct CB_PS_SpotLight
{
	DirectX::XMFLOAT3 position;
	float innerCutoff;//4
	DirectX::XMFLOAT3 direction;
	float outerCutoff;//4

	DirectX::XMFLOAT3 diffuse;
	float strength;//4
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

struct D3D11_CB_PS_Lights
{
	CB_PS_PointLight pointLights[MAX_POINT_LIGHTS_SUPPORTED];
	CB_PS_DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS_SUPPORTED];
	CB_PS_SpotLight spotLights[MAX_SPOT_LIGHTS_SUPPORTED];
};
