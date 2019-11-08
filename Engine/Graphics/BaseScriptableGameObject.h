#pragma once
#include "RenderableGameObject.h"

class BaseScriptableGameObject : public RenderableGameObject
{
public:
	virtual bool Initialize(const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_PerObject> & CB_VS_PerObject) = 0;
	virtual void Draw(const XMMATRIX & viewProjectionMatrix) = 0;
};