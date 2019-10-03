#include "GameObject.h"



const XMVECTOR & GameObject::GetPositionVector() const
{
	return this->posVector;
}

const DirectX::XMFLOAT3 & GameObject::GetPositionFloat3() const
{
	return this->pos;
}

const XMVECTOR & GameObject::GetRotationVector() const
{
	return this->rotVector;
}

const DirectX::XMFLOAT3 & GameObject::GetRotationFloat3() const
{
	return this->rot;
}

void GameObject::SetPosition(const XMVECTOR & pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	this->UpdateMatrix();
}

void GameObject::SetPosition(const DirectX::XMFLOAT3 & pos)
{
	this->pos = pos;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::SetPosition(float x, float y, float z)
{
	this->pos = DirectX::XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(const XMVECTOR & pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(const DirectX::XMFLOAT3 & pos)
{
	this->pos.x += pos.x;
	this->pos.y += pos.y;
	this->pos.z += pos.z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::SetRotation(const XMVECTOR & rot)
{
	this->rotVector = rot;
	XMStoreFloat3(&this->rot, rot);
	this->UpdateMatrix();
}

void GameObject::SetRotation(const DirectX::XMFLOAT3 & rot)
{
	this->rot = rot;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void GameObject::SetRotation(float x, float y, float z)
{
	this->rot = DirectX::XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(const XMVECTOR & rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(const DirectX::XMFLOAT3 & rot)
{
	this->rot.x += rot.x;
	this->rot.y += rot.y;
	this->rot.z += rot.x;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}
void GameObject::AdjustScale(float x, float y, float z)
{
	this->scale.x += x;
	this->scale.y += y;
	this->scale.z += z;
	this->scaleVector = XMLoadFloat3(&this->scale);
	this->UpdateMatrix();
}
void GameObject::SetScale(const XMVECTOR & scale)
{
	this->scaleVector = scale;
	XMStoreFloat3(&this->scale, scale);
	this->UpdateMatrix();
}

void GameObject::SetScale(const DirectX::XMFLOAT3 & scale)
{
	this->scale = scale;
	this->scaleVector = XMLoadFloat3(&this->scale);
	this->UpdateMatrix();
}

void GameObject::SetScale(float xScale, float yScale, float zScale)
{
	scale.x = xScale;
	scale.y = yScale;
	scale.z = zScale;
	this->scaleVector = XMLoadFloat3(&this->scale);
	UpdateMatrix();
}

void GameObject::AdjustScale(const XMVECTOR & scale)
{
	this->scaleVector += scale;
	XMStoreFloat3(&this->scale, this->scaleVector);
	this->UpdateMatrix();
}

void GameObject::AdjustScale(const DirectX::XMFLOAT3 & scale)
{
	this->scale.x += scale.x;
	this->scale.y += scale.y;
	this->scale.z += scale.x;
	this->scaleVector = XMLoadFloat3(&this->scale);
	this->UpdateMatrix();
}



void GameObject::UpdateMatrix()
{
	assert("UpdataMatrix must be overriden" && 0);
}


