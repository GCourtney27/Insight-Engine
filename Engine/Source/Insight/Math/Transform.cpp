#include "ie_pch.h"

#include "Transform.h"

namespace Insight {



	Transform::Transform(const Transform& t)
	{
		m_PositionVector = t.m_PositionVector;
		m_RotationVector = t.m_RotationVector;
		m_ScaleVector = t.m_ScaleVector;

		m_Position = t.m_Position;
		m_Rotation = t.m_Rotation;
		m_Scale = t.m_Scale;

		vec_forward = t.vec_forward;
		vec_left = t.vec_left;
		vec_right = t.vec_right;
		vec_backward = t.vec_backward;

		vec_forward_noY = t.vec_forward_noY;
		vec_left_noY = t.vec_left_noY;
		vec_right_noY = t.vec_right_noY;
		vec_backward_noY = t.vec_backward_noY;
	}

	void Transform::SetPosition(const XMVECTOR& pos)
	{
		XMStoreFloat3(&m_Position, pos);
		m_PositionVector = pos;
		UpdateMatrix();
	}

	void Transform::SetPosition(const DirectX::XMFLOAT3& pos)
	{
		m_Position = pos;
		m_PositionVector = XMLoadFloat3(&m_Position);
		UpdateMatrix();
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		m_Position = DirectX::XMFLOAT3(x, y, z);
		m_PositionVector = XMLoadFloat3(&m_Position);
		UpdateMatrix();
	}

	void Transform::AdjustPosition(const XMVECTOR& pos)
	{
		m_PositionVector += pos;
		XMStoreFloat3(&m_Position, m_PositionVector);
		UpdateMatrix();
	}

	void Transform::AdjustPosition(const DirectX::XMFLOAT3& pos)
	{
		m_Position.x += pos.x;
		m_Position.y += pos.y;
		m_Position.z += pos.z;
		m_PositionVector = XMLoadFloat3(&m_Position);
		UpdateMatrix();
	}

	void Transform::AdjustPosition(float x, float y, float z)
	{
		m_Position.x += x;
		m_Position.y += y;
		m_Position.z += z;
		m_PositionVector = XMLoadFloat3(&m_Position);
		UpdateMatrix();
	}

	void Transform::SetRotation(const XMVECTOR& rot)
	{
		m_RotationVector = rot;
		XMStoreFloat3(&m_Rotation, rot);
		UpdateMatrix();
	}

	void Transform::SetRotation(const DirectX::XMFLOAT3& rot)
	{
		m_Rotation = rot;
		m_RotationVector = XMLoadFloat3(&m_Rotation);
		UpdateMatrix();
	}

	void Transform::SetRotation(float x, float y, float z)
	{
		m_Rotation = DirectX::XMFLOAT3(x, y, z);
		m_RotationVector = XMLoadFloat3(&m_Rotation);
		UpdateMatrix();
	}

	void Transform::AdjustRotation(const XMVECTOR& rot)
	{
		m_RotationVector += rot;
		XMStoreFloat3(&m_Rotation, m_RotationVector);
		UpdateMatrix();
	}

	void Transform::AdjustRotation(const DirectX::XMFLOAT3& rot)
	{
		m_Rotation.x += rot.x;
		m_Rotation.y += rot.y;
		m_Rotation.z += rot.x;
		m_RotationVector = XMLoadFloat3(&m_Rotation);
		UpdateMatrix();
	}

	void Transform::AdjustRotation(float x, float y, float z)
	{
		m_Rotation.x += x;
		m_Rotation.y += y;
		m_Rotation.z += z;
		m_RotationVector = XMLoadFloat3(&m_Rotation);
		UpdateMatrix();
	}
	void Transform::AdjustScale(float x, float y, float z)
	{
		m_Scale.x += x;
		m_Scale.y += y;
		m_Scale.z += z;
		m_ScaleVector = XMLoadFloat3(&m_Scale);
		UpdateMatrix();
	}
	void Transform::SetScale(const XMVECTOR& scale)
	{
		m_ScaleVector = scale;
		XMStoreFloat3(&m_Scale, scale);
		UpdateMatrix();
	}

	void Transform::SetScale(const DirectX::XMFLOAT3& scale)
	{
		m_Scale = scale;
		m_ScaleVector = XMLoadFloat3(&m_Scale);
		UpdateMatrix();
	}

	void Transform::SetScale(float xScale, float yScale, float zScale)
	{
		m_Scale.x = xScale;
		m_Scale.y = yScale;
		m_Scale.z = zScale;
		m_ScaleVector = XMLoadFloat3(&m_Scale);
		UpdateMatrix();
	}

	void Transform::AdjustScale(const XMVECTOR& scale)
	{
		m_ScaleVector += scale;
		XMStoreFloat3(&m_Scale, m_ScaleVector);
		UpdateMatrix();
	}

	void Transform::AdjustScale(const DirectX::XMFLOAT3& scale)
	{
		m_Scale.x += scale.x;
		m_Scale.y += scale.y;
		m_Scale.z += scale.x;
		m_ScaleVector = XMLoadFloat3(&m_Scale);
		UpdateMatrix();
	}

	void Transform::SetLookAtPos(DirectX::XMFLOAT3 lookAtPos)
	{
		// Verify that look at pos is not the same as Model pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior
		if (lookAtPos.x == m_Position.x && lookAtPos.y == m_Position.y && lookAtPos.z == m_Position.z)
		{
			return;
		}

		lookAtPos.x = m_Position.x - lookAtPos.x;
		lookAtPos.y = m_Position.y - lookAtPos.y;
		lookAtPos.z = m_Position.z - lookAtPos.z;

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
		m_LocalMatrix = matrix;
	}

	void Transform::SetWorldMatrix(XMMATRIX matrix)
	{
		m_WorldMatrix = matrix;
	}

	const XMVECTOR& Transform::GetForwardVector(bool omitY)
	{
		if (omitY)
			return vec_forward_noY;
		else
			return vec_forward;
	}

	const XMVECTOR& Transform::GetRightVector(bool omitY)
	{
		if (omitY)
			return vec_right_noY;
		else
			return vec_right;
	}

	const XMVECTOR& Transform::GetUpVector()
	{
		return DEFAULT_UP_VECTOR;
	}

	const XMVECTOR& Transform::GetDownVector()
	{
		return DEFAULT_DOWN_VECTOR;
	}

	const XMVECTOR& Transform::GetBackwardVector(bool omitY)
	{
		if (omitY)
			return vec_backward_noY;
		else
			return vec_backward;
	}

	const XMVECTOR& Transform::GetLeftVector(bool omitY)
	{
		if (omitY)
			return vec_left_noY;
		else
			return vec_left;
	}

	void Transform::UpdateMatrix()
	{
		m_LocalMatrix = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z) *
			XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z) *
			XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);

		if (m_pParent != nullptr)
		{
			//worldMatrix = m_pParent->GetWorldMatrix() * localMatrix;

			//DirectX::XMMATRIX parentMat = m_pParent->GetWorldMatrix();
			m_WorldMatrix = DirectX::XMMatrixMultiply(m_pParent->GetWorldMatrix(), m_LocalMatrix);
		}
		else
		{
			m_WorldMatrix = m_LocalMatrix;
		}

		// ======================================================

		/*localMatrix = XMMatrix(scale, rotation, translation);
		if (parent)
		{
			worldMatrix = localMatrix * parent->worldMatrix;
		}
		else
		{
			worldMatrix = localMatrix;
		}*/
		//************ Once this is implemented disable the code below ************
		// ======================================================

		// WORKS
		//localMatrix = XMMatrixScaling(scale.x, scale.y, scale.z) * XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * XMMatrixTranslation(pos.x, pos.y, pos.z);
		UpdateDirectionVectors();
	}

	void Transform::UpdateDirectionVectors()
	{
		XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, 0.0f);
		vec_forward = XMVector3TransformCoord(DEFAULT_FOREWARD_VECTOR, vecRotationMatrix);
		vec_backward = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
		vec_left = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrix);
		vec_right = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrix);

		XMMATRIX vecRotationMatrixNoY = XMMatrixRotationRollPitchYaw(0.0f, m_Rotation.y, 0.0f);
		vec_forward_noY = XMVector3TransformCoord(DEFAULT_FOREWARD_VECTOR, vecRotationMatrixNoY);
		vec_backward_noY = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrixNoY);
		vec_left_noY = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrixNoY);
		vec_right_noY = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrixNoY);
	}

}
