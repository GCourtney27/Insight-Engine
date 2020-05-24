#include <ie_pch.h>


#include "ACamera.h"


namespace Insight {



	Camera::~Camera()
	{
	}

	void Camera::ProcessMouseScroll(float yOffset)
	{
	}

	void Camera::ProcessMouseMovement(float xPos, float yPos)
	{
		GetTransformRef().Rotate(yPos * m_MouseSensitivity, xPos * m_MouseSensitivity, 0.0f);

		UpdateViewMatrix();
		GetTransformRef().UpdateLocalDirectionVectors();
	}
	
	void Camera::ProcessKeyboardInput(CameraMovement direction, float deltaTime)
	{
		float velocity = m_MovementSpeed * deltaTime;
		if (direction == FORWARD)
		{
			GetTransformRef().GetPositionRef() += GetTransformRef().GetLocalForward() * velocity;
		}
		if (direction == BACKWARD)
		{
			GetTransformRef().GetPositionRef() -= GetTransformRef().GetLocalForward() * velocity;
		}
		if (direction == LEFT)
		{
			GetTransformRef().GetPositionRef() -= GetTransformRef().GetLocalRight() * velocity;
		}
		if (direction == RIGHT)
		{
			GetTransformRef().GetPositionRef() += GetTransformRef().GetLocalRight() * velocity;
		}
		if (direction == UP)
		{
			GetTransformRef().GetPositionRef() += WORLD_DIRECTION.Up * velocity;
		}
		if (direction == DOWN)
		{
			GetTransformRef().GetPositionRef() -= WORLD_DIRECTION.Up * velocity;
		}
		UpdateViewMatrix();
	}
	
	void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
	{
		m_Fov = fovDegrees;
		m_NearZ = nearZ;
		m_FarZ = farZ;
		m_AspectRatio = aspectRatio;
		float fovRadians = fovDegrees * (3.14f / 180.0f);
		m_ProjectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, m_AspectRatio, m_NearZ, m_FarZ);
	}

	void Camera::UpdateViewMatrix()
	{
		XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(GetTransformRef().GetRotation().x, GetTransformRef().GetRotation().y, 0.0f);
		XMVECTOR camTarget = XMVector3TransformCoord(WORLD_DIRECTION.Forward, camRotationMatrix);
		camTarget += GetTransformRef().GetPosition();
		XMVECTOR upDir = XMVector3TransformCoord(WORLD_DIRECTION.Up, camRotationMatrix);
		m_ViewMatrix = XMMatrixLookAtLH(GetTransformRef().GetPosition(), camTarget, upDir);

	}
}