#include "Light.h"

bool Light::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_PerObject>& CB_VS_PerObject)
{
	if (!model.Initialize("Data/Objects/light.fbx", device, deviceContext, CB_VS_PerObject, nullptr))
		return false;
	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}
