#include <Engine_pch.h>

#include "Runtime/Math/Public/Transform.h"
#include "Runtime/Core/Application.h"

namespace Insight {



	ieTransform::ieTransform()
		: m_Position(FVector3::Zero)
		, m_Rotation(FQuat::Identity)
		, m_Scale(FVector3::One)
		, m_EditorPlayOriginPosition(m_Position)
		, m_EditorPlayOriginRotation(m_Rotation)
		, m_EditorPlayOriginScale(m_Scale)
	{
	}

	void ieTransform::EditorEndPlay()
	{
		m_Position = m_EditorPlayOriginPosition;
		m_Rotation = m_EditorPlayOriginRotation;
		m_Scale = m_EditorPlayOriginScale;
		UpdateIfTransformed(true);
	}

	void ieTransform::LookAt(FVector3& target)
	{
		// Verify that look at pos is not the same as Model pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior
		if (target == m_Position)
		{
			return;
		}

		float pitch = 0.0f;
		if (target.y != 0.0f)
		{
			const float distance = sqrt(target.x * target.x + target.z * target.z);
			pitch = atanf(target.y / distance);
		}

		float yaw = 0.0f;
		if (target.x != 0.0f)
		{
			yaw = atanf(target.x / target.z);
		}
		if (target.z > 0)
		{
			yaw += DirectX::XM_PI;
		}

		SetRotation(pitch, yaw, 0.0f);
	}

	void ieTransform::SetLocalMatrix(const FMatrix& matrix)
	{
		m_LocalMatrix = matrix;
	}

	void ieTransform::SetWorldMatrix(const FMatrix& matrix)
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
	}

	void ieTransform::TranslateLocalMatrix()
	{
		m_TranslationMat = DirectX::XMMatrixTranslationFromVector(m_Position);
	}

	void ieTransform::ScaleLocalMatrix()
	{
		m_ScaleMat = DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	}

	void ieTransform::RotateLocalMatrix()
	{
		m_RotationMat = DirectX::XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
		RotateVector(m_LocalForward, FVector3::Forward, m_RotationMat);
		RotateVector(m_LocalBackward, FVector3::Backward, m_RotationMat);
		RotateVector(m_LocalLeft, FVector3::Left, m_RotationMat);
		RotateVector(m_LocalRight, FVector3::Right, m_RotationMat);
		RotateVector(m_LocalUp, FVector3::Up, m_RotationMat);
		RotateVector(m_LocalDown, FVector3::Down, m_RotationMat);
	}

	void ieTransform::UpdateEditorOriginPositionRotationScale()
	{
		m_EditorPlayOriginPosition = m_Position;
		m_EditorPlayOriginRotation = m_Rotation;
		m_EditorPlayOriginScale = m_Scale;
	}

}
