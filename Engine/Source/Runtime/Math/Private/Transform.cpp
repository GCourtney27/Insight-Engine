#include <Engine_pch.h>

#include "Runtime/Math/Public/Transform.h"
#include "Runtime/Core/Public/Application.h"

namespace Insight {


	void ieTransform::LookAt(const FVector3& target)
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

	void ieTransform::UpdateLocalMatrix()
	{
		m_LocalMatrix = m_ScaleMat * m_TranslationMat * m_RotationMat;
		ComputeWorldMatrix();
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
	}
}
