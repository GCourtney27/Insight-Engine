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
		FVector Offset(x, y, z);
		m_Position += Offset;
		TranslateLocalMatrix();
		UpdateLocalMatrix();
	}

	void ieTransform::Rotate(float XInDegrees, float YInDegrees, float ZInDegrees)
	{
		FVector Offset(DEGREES_TO_RADIANS(XInDegrees), DEGREES_TO_RADIANS(YInDegrees), DEGREES_TO_RADIANS(ZInDegrees));
		m_Rotation += Offset;
		RotateLocalMatrix();
		UpdateLocalMatrix();
	}

	void ieTransform::Scale(float x, float y, float z)
	{
		FVector Offset(x, y, z);
		m_Scale += Offset; 
		ScaleLocalMatrix();
		UpdateLocalMatrix();
	}

	void ieTransform::LookAt( FVector& target)
	{
		// Verify that look at pos is not the same as Model pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior
		if (target == m_Position)
		{
			return;
		}

		float pitch = 0.0f;
		if (target.Y() != 0.0f)
		{
			const float distance = sqrt(target.X() * target.X() + target.Z() * target.Z());
			pitch = atanf(target.Y() / distance);
		}

		float yaw = 0.0f;
		if (target.X() != 0.0f)
		{
			yaw = atanf(target.X() / target.Z());
		}
		if (target.Z() > 0)
		{
			yaw += DirectX::XM_PI;
		}

		SetRotation( (pitch, yaw, 0.0f) );
	}

	void ieTransform::SetDirection(FVector& NewDirection)
	{
		DirectX::XMMATRIX vecRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(NewDirection.X(), NewDirection.Y(), NewDirection.Z());
		m_LocalForward = DirectX::XMVector3TransformCoord(const_cast<FVector*>(&FVector::Forward)->Data(), vecRotationMatrix);
		m_LocalBackward = DirectX::XMVector3TransformCoord(const_cast<FVector*>(&FVector::Backward)->Data(), vecRotationMatrix);
		m_LocalLeft = DirectX::XMVector3TransformCoord(const_cast<FVector*>(&FVector::Left)->Data(), vecRotationMatrix);
		m_LocalRight = DirectX::XMVector3TransformCoord(const_cast<FVector*>(&FVector::Right)->Data(), vecRotationMatrix);
		m_LocalUp = DirectX::XMVector3TransformCoord(const_cast<FVector*>(&FVector::Up)->Data(), vecRotationMatrix);
		m_LocalDown = DirectX::XMVector3TransformCoord(const_cast<FVector*>(&FVector::Down)->Data(), vecRotationMatrix);
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
		m_TranslationMat = DirectX::XMMatrixTranslationFromVector(m_Position.Data());
	}

	void ieTransform::ScaleLocalMatrix()
	{
		DirectX::XMMATRIX position = m_TranslationMat;
		// send to origin
		m_TranslationMat = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		// scale
		m_ScaleMat = DirectX::XMMatrixScaling(m_Scale.X(), m_Scale.Y(), m_Scale.Z());
		// send back to position
		m_TranslationMat = position;

	}

	void ieTransform::RotateLocalMatrix()
	{
		m_RotationMat = DirectX::XMMatrixRotationRollPitchYaw(m_Rotation.X(), m_Rotation.Y(), m_Rotation.Z());
	}

	void ieTransform::UpdateLocalDirectionVectors()
	{
		ieMatrix vecRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(m_Rotation.X(), m_Rotation.Y(), m_Rotation.Z());
		m_LocalForward = DirectX::XMVector3TransformCoord(const_cast<FVector*>(&FVector::Forward)->Data(), m_RotationMat);
		m_LocalBackward = DirectX::XMVector3TransformCoord(const_cast<FVector*>(&FVector::Backward)->Data(), m_RotationMat);
		m_LocalLeft = DirectX::XMVector3TransformCoord(const_cast<FVector*>(&FVector::Left)->Data(), m_RotationMat);
		m_LocalRight = DirectX::XMVector3TransformCoord(const_cast<FVector*>(&FVector::Right)->Data(), m_RotationMat);
		m_LocalUp = DirectX::XMVector3TransformCoord(const_cast<FVector*>(&FVector::Up)->Data(), m_RotationMat);
		m_LocalDown = DirectX::XMVector3TransformCoord(const_cast<FVector*>(&FVector::Down)->Data(), m_RotationMat);
	}

	void ieTransform::UpdateEditorOriginPositionRotationScale()
	{
		m_EditorPlayOriginPosition = m_Position;
		m_EditorPlayOriginRotation = m_Rotation;
		m_EditorPlayOriginScale = m_Scale;
	}

}
