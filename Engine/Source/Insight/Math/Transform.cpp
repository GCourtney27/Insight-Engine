#include <ie_pch.h>

#include "Transform.h"
#include "Insight/Core/Application.h"

namespace Insight {



	ieTransform::ieTransform()
	{
	}

	ieTransform::~ieTransform()
	{
	}

	ieTransform::ieTransform(const ieTransform& t)
	{
		m_Position = t.m_Position;
		m_Rotation = t.m_Rotation;
		m_Scale = t.m_Scale;
	}

	ieTransform::ieTransform(ieTransform&& transform) noexcept
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

	ieTransform& ieTransform::operator=(const ieTransform& transform)
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

	void ieTransform::EditorEndPlay()
	{
		m_Position = m_EditorPlayOriginPosition;
		m_Rotation = m_EditorPlayOriginRotation;
		m_Scale = m_EditorPlayOriginScale;

		UpdateIfTransformed(true);
	}

	void ieTransform::Translate(float x, float y, float z)
	{
		m_Position.x += x; 
		m_Position.y += y; 
		m_Position.z += z; 
		TranslateLocalMatrix();
		UpdateLocalMatrix();
	}

	void ieTransform::Rotate(float XInDegrees, float YInDegrees, float ZInDegrees)
	{
		m_Rotation.x += DEGREES_TO_RADIANS(XInDegrees);
		m_Rotation.y += DEGREES_TO_RADIANS(YInDegrees);
		m_Rotation.z += DEGREES_TO_RADIANS(ZInDegrees);
		RotateLocalMatrix();
		UpdateLocalMatrix();
	}

	void ieTransform::Scale(float x, float y, float z)
	{
		m_Scale.x += x; 
		m_Scale.y += y; 
		m_Scale.z += z;
		ScaleLocalMatrix();
		UpdateLocalMatrix();
	}

	void ieTransform::LookAt(const ieVector3& target)
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

		SetRotation(ieVector3(pitch, yaw, 0.0f));
	}

	void ieTransform::SetDirection(const ieVector3& NewDirection)
	{
		XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(NewDirection.x, NewDirection.y, NewDirection.z);
		m_LocalForward = XMVector3TransformCoord(Vector3::Forward, vecRotationMatrix);
		m_LocalBackward = XMVector3TransformCoord(Vector3::Backward, vecRotationMatrix);
		m_LocalLeft = XMVector3TransformCoord(Vector3::Left, vecRotationMatrix);
		m_LocalRight = XMVector3TransformCoord(Vector3::Right, vecRotationMatrix);
		m_LocalUp = XMVector3TransformCoord(Vector3::Up, vecRotationMatrix);
		m_LocalDown = XMVector3TransformCoord(Vector3::Down, vecRotationMatrix);
	}

	void ieTransform::SetLocalMatrix(ieMatrix matrix)
	{
		m_LocalMatrix = matrix;
	}

	void ieTransform::SetWorldMatrix(ieMatrix matrix)
	{
		m_WorldMatrix = matrix;
	}

	void ieTransform::UpdateIfTransformed(bool ForceUpdate)
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

	void ieTransform::UpdateLocalMatrix()
	{
		m_LocalMatrix = m_ScaleMat * m_TranslationMat * m_RotationMat;
		UpdateLocalDirectionVectors();
	}

	void ieTransform::TranslateLocalMatrix()
	{
		m_TranslationMat = XMMatrixTranslationFromVector(m_Position);
	}

	void ieTransform::ScaleLocalMatrix()
	{
		XMMATRIX position = m_TranslationMat;
		// send to origin
		m_TranslationMat = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		// scale
		m_ScaleMat = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		// send back to position
		m_TranslationMat = position;

	}

	void ieTransform::RotateLocalMatrix()
	{
		m_RotationMat = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	}

	void ieTransform::UpdateLocalDirectionVectors()
	{
		ieMatrix vecRotationMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
		m_LocalForward = XMVector3TransformCoord(Vector3::Forward, m_RotationMat);
		m_LocalBackward = XMVector3TransformCoord(Vector3::Backward, m_RotationMat);
		m_LocalLeft = XMVector3TransformCoord(Vector3::Left, m_RotationMat);
		m_LocalRight = XMVector3TransformCoord(Vector3::Right, m_RotationMat);
		m_LocalUp = XMVector3TransformCoord(Vector3::Up, m_RotationMat);
		m_LocalDown = XMVector3TransformCoord(Vector3::Down, m_RotationMat);
	}

	void ieTransform::UpdateEditorOriginPositionRotationScale()
	{
		m_EditorPlayOriginPosition = m_Position;
		m_EditorPlayOriginRotation = m_Rotation;
		m_EditorPlayOriginScale = m_Scale;
	}

}
