#include "RenderableGameObject.h"

bool RenderableGameObject::Initialize(const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!model.Initialize(filepath, device, deviceContext, cb_vs_vertexshader))
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();

	//sphere_radius = 20.0f;
	//sphere_position = GetPositionFloat3();

	aabb.Initialize(20.0f, GetPositionFloat3());

	return true;
}

void RenderableGameObject::Draw(const XMMATRIX & viewProjectionMatrix)
{
	model.Draw(this->worldMatrix, viewProjectionMatrix);
	
	
}

void RenderableGameObject::Update()
{
	AdjustPosition(0.0f, 0.0f, 0.0f);
	// TODO: Update sphere collider (Move this somewhere else)
	sphere_position = GetPositionFloat3();
	aabb.SetPosition(GetPositionFloat3());
}


void RenderableGameObject::UpdateMatrix()
{
	this->worldMatrix = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z) * XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	this->UpdateDirectionVectors();
}