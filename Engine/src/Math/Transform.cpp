#include "Transform.h"


Transform::Transform(const Transform & t)
{
	posVector = t.posVector;
	rotVector = t.rotVector;
	scaleVector = t.scaleVector;

	pos = t.pos;
	rot = t.rot;
	scale = t.scale;

	vec_forward = t.vec_forward;
	vec_left = t.vec_left;
	vec_right = t.vec_right;
	vec_backward = t.vec_backward;

	vec_forward_noY = t.vec_forward_noY;
	vec_left_noY = t.vec_left_noY;
	vec_right_noY = t.vec_right_noY;
	vec_backward_noY = t.vec_backward_noY;
}

const XMVECTOR & Transform::GetPositionVector() const
{
	return this->posVector;
}

const DirectX::XMFLOAT3 & Transform::GetPositionFloat3() const
{
	return this->pos;
}

const XMVECTOR & Transform::GetRotationVector() const
{
	return this->rotVector;
}

const DirectX::XMFLOAT3 & Transform::GetRotationFloat3() const
{
	return this->rot;
}

const XMVECTOR & Transform::GetScaleVector() const
{
	return this->scaleVector;
}

const XMFLOAT3 & Transform::GetScaleFloat3() const
{
	return this->scale;
}

void Transform::SetPosition(const XMVECTOR & pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	this->UpdateMatrix();
}

void Transform::SetPosition(const DirectX::XMFLOAT3 & pos)
{
	this->pos = pos;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void Transform::SetPosition(float x, float y, float z)
{
	this->pos = DirectX::XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void Transform::AdjustPosition(const XMVECTOR & pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateMatrix();
}

void Transform::AdjustPosition(const DirectX::XMFLOAT3 & pos)
{
	this->pos.x += pos.x;
	this->pos.y += pos.y;
	this->pos.z += pos.z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void Transform::AdjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void Transform::SetRotation(const XMVECTOR & rot)
{
	this->rotVector = rot;
	XMStoreFloat3(&this->rot, rot);
	this->UpdateMatrix();
}

void Transform::SetRotation(const DirectX::XMFLOAT3 & rot)
{
	this->rot = rot;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void Transform::SetRotation(float x, float y, float z)
{
	this->rot = DirectX::XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void Transform::AdjustRotation(const XMVECTOR & rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateMatrix();
}

void Transform::AdjustRotation(const DirectX::XMFLOAT3 & rot)
{
	this->rot.x += rot.x;
	this->rot.y += rot.y;
	this->rot.z += rot.x;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void Transform::AdjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}
void Transform::AdjustScale(float x, float y, float z)
{
	this->scale.x += x;
	this->scale.y += y;
	this->scale.z += z;
	this->scaleVector = XMLoadFloat3(&this->scale);
	this->UpdateMatrix();
}
void Transform::SetScale(const XMVECTOR & scale)
{
	this->scaleVector = scale;
	XMStoreFloat3(&this->scale, scale);
	this->UpdateMatrix();
}

void Transform::SetScale(const DirectX::XMFLOAT3 & scale)
{
	this->scale = scale;
	this->scaleVector = XMLoadFloat3(&this->scale);
	this->UpdateMatrix();
}

void Transform::SetScale(float xScale, float yScale, float zScale)
{
	scale.x = xScale;
	scale.y = yScale;
	scale.z = zScale;
	this->scaleVector = XMLoadFloat3(&this->scale);
	UpdateMatrix();
}

void Transform::AdjustScale(const XMVECTOR & scale)
{
	this->scaleVector += scale;
	XMStoreFloat3(&this->scale, this->scaleVector);
	this->UpdateMatrix();
}

void Transform::AdjustScale(const DirectX::XMFLOAT3 & scale)
{
	this->scale.x += scale.x;
	this->scale.y += scale.y;
	this->scale.z += scale.x;
	this->scaleVector = XMLoadFloat3(&this->scale);
	this->UpdateMatrix();
}

void Transform::SetLookAtPos(DirectX::XMFLOAT3 lookAtPos)
{
	// Verify that look at pos is not the same as Model pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior
	if (lookAtPos.x == pos.x && lookAtPos.y == pos.y && lookAtPos.z == pos.z)
	{
		return;
	}

	lookAtPos.x = pos.x - lookAtPos.x;
	lookAtPos.y = pos.y - lookAtPos.y;
	lookAtPos.z = pos.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
	{
		yaw += XM_PI;
	}

	SetRotation(pitch, yaw, 0.0f);
}

void Transform::SetLocalMatrix(XMMATRIX matrix)
{
	this->localMatrix = matrix;
}

void Transform::SetWorldMatrix(XMMATRIX matrix)
{
	this->worldMatrix = matrix;
}

const XMVECTOR & Transform::GetForwardVector(bool omitY)
{
	if (omitY)
		return vec_forward_noY;
	else
		return vec_forward;
}

const XMVECTOR & Transform::GetRightVector(bool omitY)
{
	if (omitY)
		return vec_right_noY;
	else
		return vec_right;
}

const XMVECTOR & Transform::GetUpVector()
{
	return DEFAULT_UP_VECTOR;
}

const XMVECTOR & Transform::GetDownVector()
{
	return DEFAULT_DOWN_VECTOR;
}

const XMVECTOR & Transform::GetBackwardVector(bool omitY)
{
	if (omitY)
		return vec_backward_noY;
	else
		return vec_backward;
}

const XMVECTOR & Transform::GetLeftVector(bool omitY)
{
	if (omitY)
		return vec_left_noY;
	else
		return vec_left;
}

void Transform::UpdateMatrix()
{
	this->localMatrix = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z) *
						XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) *
						XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);

	if (m_pParent != nullptr)
	{
		//this->worldMatrix = m_pParent->GetWorldMatrix() * this->localMatrix;
		
		//DirectX::XMMATRIX parentMat = m_pParent->GetWorldMatrix();
		this->worldMatrix = DirectX::XMMatrixMultiply(m_pParent->GetWorldMatrix(), this->localMatrix);
	}
	else
	{
		this->worldMatrix = this->localMatrix;
	}

	// ======================================================

	/*this->localMatrix = XMMatrix(scale, rotation, translation);
	if (parent)
	{
		this->worldMatrix = this->localMatrix * parent->worldMatrix;
	}
	else
	{
		this->worldMatrix = this->localMatrix;
	}*/
	//************ Once this is implemented disable the code below ************
	// ======================================================

	// WORKS
	//this->localMatrix = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z) * XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	this->UpdateDirectionVectors();
}

void Transform::UpdateDirectionVectors()
{
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, 0.0f);
	vec_forward = XMVector3TransformCoord(DEFAULT_FOREWARD_VECTOR, vecRotationMatrix);
	vec_backward = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	vec_left = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	vec_right = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrix);

	XMMATRIX vecRotationMatrixNoY = XMMatrixRotationRollPitchYaw(0.0f, rot.y, 0.0f);
	vec_forward_noY = XMVector3TransformCoord(DEFAULT_FOREWARD_VECTOR, vecRotationMatrixNoY);
	vec_backward_noY = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrixNoY);
	vec_left_noY = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrixNoY);
	vec_right_noY = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrixNoY);
}
