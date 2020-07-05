#include <ie_pch.h>

#include "Transform.h"
#include "Insight/Core/Application.h"

namespace Insight {



	Transform::Transform()
	{
	}

	Transform::~Transform()
	{
	}

	Transform::Transform(const Transform& t)
	{
		m_Position = t.m_Position;
		m_Rotation = t.m_Rotation;
		m_Scale = t.m_Scale;
	}

	Transform::Transform(Transform&& transform) noexcept
	{
		m_LocalMatrix = transform.m_LocalMatrix;
		m_WorldMatrix = transform.m_WorldMatrix;

		m_TranslationMat = transform.m_TranslationMat;
		m_RotationMat = transform.m_RotationMat;
		m_ScaleMat = transform.m_ScaleMat;

		m_Position = transform.m_Position;
		m_Rotation = transform.m_Rotation;
		m_Scale = transform.m_Scale;

		m_LocalForward = transform.m_LocalForward;
		m_LocalBackward = transform.m_LocalBackward;
		m_LocalLeft = transform.m_LocalLeft;
		m_LocalRight = transform.m_LocalRight;
		m_LocalUp = transform.m_LocalUp;
		m_LocalDown = transform.m_LocalDown;
	}

	Transform& Transform::operator=(const Transform& transform)
	{
		// Vectors
		m_Position = transform.m_Position;
		m_Rotation = transform.m_Rotation;
		m_Scale = transform.m_Scale;
		m_LocalForward = transform.m_LocalForward;
		m_LocalBackward = transform.m_LocalBackward;
		m_LocalLeft = transform.m_LocalLeft;
		m_LocalRight = transform.m_LocalRight;
		m_LocalUp = transform.m_LocalUp;
		m_LocalDown = transform.m_LocalDown;

		// Matricies
		m_LocalMatrix = transform.m_LocalMatrix;
		m_WorldMatrix = transform.m_WorldMatrix;
		m_TranslationMat = transform.m_TranslationMat;
		m_ScaleMat = transform.m_ScaleMat;
		m_RotationMat = transform.m_RotationMat;

		return *this;
	}

	void Transform::EditorEndPlay()
	{
		m_Position = m_EditorPlayOriginPosition;
		m_Rotation = m_EditorPlayOriginRotation;
		m_Scale = m_EditorPlayOriginScale;

		UpdateIfTransformed(true);
	}

	void Transform::Translate(float x, float y, float z)
	{
		m_Position.x += x; 
		m_Position.y += y; 
		m_Position.z += z; 
		TranslateLocalMatrix();
		UpdateLocalMatrix();
	}

	void Transform::Rotate(float XInDegrees, float YInDegrees, float ZInDegrees)
	{
		m_Rotation.x += DEGREES_TO_RADIANS(XInDegrees);
		m_Rotation.y += DEGREES_TO_RADIANS(YInDegrees);
		m_Rotation.z += DEGREES_TO_RADIANS(ZInDegrees);
		RotateLocalMatrix();
		UpdateLocalMatrix();
	}

	void Transform::Scale(float x, float y, float z)
	{
		m_Scale.x += x; 
		m_Scale.y += y; 
		m_Scale.z += z;
		ScaleLocalMatrix();
		UpdateLocalMatrix();
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

	void Transform::SetDirection(const Vector3& NewDirection)
	{
		XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(NewDirection.x, NewDirection.y, NewDirection.z);
		m_LocalForward = XMVector3TransformCoord(WORLD_DIRECTION.Forward, vecRotationMatrix);
		m_LocalBackward = XMVector3TransformCoord(WORLD_DIRECTION.Backward, vecRotationMatrix);
		m_LocalLeft = XMVector3TransformCoord(WORLD_DIRECTION.Left, vecRotationMatrix);
		m_LocalRight = XMVector3TransformCoord(WORLD_DIRECTION.Right, vecRotationMatrix);
		m_LocalUp = XMVector3TransformCoord(WORLD_DIRECTION.Up, vecRotationMatrix);
		m_LocalDown = XMVector3TransformCoord(WORLD_DIRECTION.Down, vecRotationMatrix);
	}

	void Transform::SetLocalMatrix(XMMATRIX matrix)
	{
		m_LocalMatrix = matrix;
	}

	void Transform::SetWorldMatrix(XMMATRIX matrix)
	{
		m_WorldMatrix = matrix;
	}

	void Transform::UpdateIfTransformed(bool ForceUpdate)
	{
		if ((m_Transformed && !Application::Get().IsPlaySessionUnderWay()) || ForceUpdate)
		{
			TranslateLocalMatrix();
			ScaleLocalMatrix();
			RotateLocalMatrix();

			UpdateLocalMatrix();
			UpdateEditorOriginPositionRotationScale();

			m_Transformed = false;
		}
	}

	void Transform::UpdateLocalMatrix()
	{
		m_LocalMatrix = m_ScaleMat * m_TranslationMat * m_RotationMat;
		UpdateLocalDirectionVectors();
	}

	void Transform::TranslateLocalMatrix()
	{
		m_TranslationMat = XMMatrixTranslationFromVector(m_Position);
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
		m_RotationMat = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	}

	void Transform::UpdateLocalDirectionVectors()
	{
		//XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
		m_LocalForward = XMVector3TransformCoord(WORLD_DIRECTION.Forward, m_RotationMat);
		m_LocalBackward = XMVector3TransformCoord(WORLD_DIRECTION.Backward, m_RotationMat);
		m_LocalLeft = XMVector3TransformCoord(WORLD_DIRECTION.Left, m_RotationMat);
		m_LocalRight = XMVector3TransformCoord(WORLD_DIRECTION.Right, m_RotationMat);
		m_LocalUp = XMVector3TransformCoord(WORLD_DIRECTION.Up, m_RotationMat);
		m_LocalDown = XMVector3TransformCoord(WORLD_DIRECTION.Down, m_RotationMat);
	}

	void Transform::UpdateEditorOriginPositionRotationScale()
	{
		m_EditorPlayOriginPosition = m_Position;
		m_EditorPlayOriginRotation = m_Rotation;
		m_EditorPlayOriginScale = m_Scale;
	}

}
