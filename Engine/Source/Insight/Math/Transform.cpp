#include "ie_pch.h"

#include "Transform.h"

namespace Insight {



	Transform::Transform()
	{

	}

	Transform::Transform(const Transform& t)
	{
		m_Position = t.m_Position;
		m_Rotation = t.m_Rotation;
		m_Scale = t.m_Scale;
	}

	void Transform::Translate(float x, float y, float z)
	{
		m_Position.x += x; 
		m_Position.y += y; 
		m_Position.z += z; 
		TranslateLocalMatrix();
		UpdateMatrix();
	}

	void Transform::Rotate(float x, float y, float z)
	{
		m_Rotation.x += x;
		m_Rotation.y += y;
		m_Rotation.z += z;
		RotateLocalMatrix();
		UpdateMatrix();
	}

	void Transform::Scale(float x, float y, float z)
	{
		m_Scale.x += x; 
		m_Scale.y += y; 
		m_Scale.z += z;
		ScaleLocalMatrix();
		UpdateMatrix();
	}

	void Transform::LookAt(const Vector3& target)
	{
		// Verify that look at pos is not the same as Model pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior
		if (target.x == m_Position.x && target.y == m_Position.y && target.z == m_Position.z)
		{
			return;
		}

		float pitch = 0.0f;
		if (target.y != 0.0f)
		{
			const float distance = sqrt(target.x * target.x + target.z * target.z);
			pitch = atan(target.y / distance);
		}

		float yaw = 0.0f;
		if (target.x != 0.0f)
		{
			yaw = atan(target.x / target.z);
		}
		if (target.z > 0)
		{
			yaw += XM_PI;
		}

		SetRotation(Vector3(pitch, yaw, 0.0f));
	}

	void Transform::SetLocalMatrix(XMMATRIX matrix)
	{
		m_LocalMatrix = matrix;
	}

	void Transform::SetWorldMatrix(XMMATRIX matrix)
	{
		m_WorldMatrix = matrix;
	}

	void Transform::UpdateMatrix()
	{
		m_LocalMatrix = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z) *
			XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z) *
			XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);

		UpdateLocalDirectionVectors();
	}

	void Transform::TranslateLocalMatrix()
	{
		m_TranslationMat = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	}

	void Transform::ScaleLocalMatrix()
	{
		XMMATRIX position = m_TranslationMat;
		// send to origin
		m_TranslationMat = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		// scale
		m_ScaleMat = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		// send back to position
		m_TranslationMat = position;

	}

	void Transform::RotateLocalMatrix()
	{
		m_LocalMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	}

	void Transform::UpdateLocalDirectionVectors()
	{
		XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
		m_LocalForward = XMVector3TransformCoord(WORLD_DIRECTION.Forward, vecRotationMatrix);
		m_LocalBackward = XMVector3TransformCoord(WORLD_DIRECTION.Backward, vecRotationMatrix);
		m_LocalLeft = XMVector3TransformCoord(WORLD_DIRECTION.Left, vecRotationMatrix);
		m_LocalRight = XMVector3TransformCoord(WORLD_DIRECTION.Right, vecRotationMatrix);
		m_LocalUp = XMVector3TransformCoord(WORLD_DIRECTION.Up, vecRotationMatrix);
		m_LocalDown = XMVector3TransformCoord(WORLD_DIRECTION.Down, vecRotationMatrix);
	}

}
