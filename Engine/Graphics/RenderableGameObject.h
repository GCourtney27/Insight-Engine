#pragma once
#include "GameObject3D.h"
#include <SimpleMath.h>

class RenderableGameObject : public GameObject3D
{
public:
	RenderableGameObject() {}
	bool Initialize(const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader); //float boundingSphere scale
	void Draw(const XMMATRIX & viewProjectionMatrix, const XMMATRIX & viewMatrix);
	
	void Update();

	Model* GetModel() { return &model; }

	/*AABB aabb;*/

	SimpleMath::Vector3 sphere_position;
	float sphere_radius = 0.0f;

protected:
	Model model;
	void UpdateMatrix() override;

	XMMATRIX worldMatrix = XMMatrixIdentity();

};