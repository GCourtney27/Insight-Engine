#include <Engine_pch.h>

#include "Runtime/Math/Public/Transform.h"
#include "Runtime/Core/Application.h"

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
		FVector3 Offset(x, y, z);
		m_Position += Offset;
		TranslateLocalMatrix();
		UpdateLocalMatrix();
	}

	void ieTransform::Rotate(float XInDegrees, float YInDegrees, float ZInDegrees)
	{
		
		FVector3 Offset(XMConvertToRadians(XInDegrees), XMConvertToRadians(YInDegrees), XMConvertToRadians(ZInDegrees));
		m_Rotation += Offset;
		RotateLocalMatrix();
		UpdateLocalMatrix();
	}

	void ieTransform::Scale(float x, float y, float z)
	{
		FVector3 Offset(x, y, z);
		m_Scale += Offset; 
		ScaleLocalMatrix();
		UpdateLocalMatrix();
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

	void ieTransform::SetDirection(FVector3& NewDirection)
	{
		DirectX::XMMATRIX vecRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(NewDirection.x, NewDirection.y, NewDirection.z);
		m_LocalForward = DirectX::XMVector3TransformCoord(FVector3::Forward, vecRotationMatrix);
		m_LocalBackward = DirectX::XMVector3TransformCoord(FVector3::Backward, vecRotationMatrix);
		m_LocalLeft = DirectX::XMVector3TransformCoord(FVector3::Left, vecRotationMatrix);
		m_LocalRight = DirectX::XMVector3TransformCoord(FVector3::Right, vecRotationMatrix);
		m_LocalUp = DirectX::XMVector3TransformCoord(FVector3::Up, vecRotationMatrix);
		m_LocalDown = DirectX::XMVector3TransformCoord(FVector3::Down, vecRotationMatrix);
	}

	void ieTransform::SetLocalMatrix(FMatrix matrix)
	{
		m_LocalMatrix = matrix;
	}

	void ieTransform::SetWorldMatrix(FMatrix matrix)
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
		m_TranslationMat = DirectX::XMMatrixTranslationFromVector(m_Position);
	}

	void ieTransform::ScaleLocalMatrix()
	{
		DirectX::XMMATRIX position = m_TranslationMat;
		// send to origin
		m_TranslationMat = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		// scale
		m_ScaleMat = DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		// send back to position
		m_TranslationMat = position;

	}

	void ieTransform::RotateLocalMatrix()
	{
		m_RotationMat = DirectX::XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	}

	void ieTransform::UpdateLocalDirectionVectors()
	{
		FMatrix vecRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
		m_LocalForward = DirectX::XMVector3TransformCoord(FVector3::Forward, m_RotationMat);
		m_LocalBackward = DirectX::XMVector3TransformCoord(FVector3::Backward, m_RotationMat);
		m_LocalLeft = DirectX::XMVector3TransformCoord(FVector3::Left, m_RotationMat);
		m_LocalRight = DirectX::XMVector3TransformCoord(FVector3::Right, m_RotationMat);
		m_LocalUp = DirectX::XMVector3TransformCoord(FVector3::Up, m_RotationMat);
		m_LocalDown = DirectX::XMVector3TransformCoord(FVector3::Down, m_RotationMat);
	}

	void ieTransform::UpdateEditorOriginPositionRotationScale()
	{
		m_EditorPlayOriginPosition = m_Position;
		m_EditorPlayOriginRotation = m_Rotation;
		m_EditorPlayOriginScale = m_Scale;
	}

}
