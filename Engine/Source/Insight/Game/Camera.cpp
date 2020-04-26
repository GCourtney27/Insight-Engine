#include "ie_pch.h"


#include "Camera.h"

#include "imgui.h"


namespace Insight {



	Camera::~Camera()
	{
	}

	void Camera::ProcessMouseScroll(float yOffset)
	{
	}

	void Camera::ProcessMouseMovement(float xPos, float yPos)
	{
		if (m_FirstMove)
		{
			m_LastLookX = xPos;
			m_LastLookY = yPos;
			m_FirstMove = false;
		}

		float xOffset = xPos - m_LastLookX;
		float yOffset = yPos - m_LastLookY;

		m_LastLookX = xPos;
		m_LastLookY = yPos;

		xOffset *= m_MouseSensitivity;
		yOffset *= m_MouseSensitivity;
		
		m_Yaw += xOffset;
		m_Pitch += yOffset;

		if (m_ConstrainPitch)
		{
			if (m_Pitch > 89.0f)
				m_Pitch = 89.0f;
			if (m_Pitch < -89.9f)
				m_Pitch = -89.0f;
		}

		m_Transform.SetRotation(Vector3(m_Pitch, m_Yaw, 0.0f));

		UpdateViewMatrix();
	}
	
	void Camera::ProcessKeyboardInput(CameraMovement direction, float deltaTime)
	{
		float velocity = m_MovementSpeed * deltaTime;
		if (direction == FORWARD)
		{
			m_Transform.GetPositionRef() += m_Transform.GetLocalForward() * velocity;
		}
		if (direction == BACKWARD)
		{
			m_Transform.GetPositionRef() -= m_Transform.GetLocalForward() * velocity;
		}
		if (direction == LEFT)
		{
			m_Transform.GetPositionRef() -= m_Transform.GetLocalRight() * velocity;
		}
		if (direction == RIGHT)
		{
			m_Transform.GetPositionRef() += m_Transform.GetLocalRight() * velocity;
		}
		if (direction == UP)
		{
			m_Transform.GetPositionRef() += WORLD_DIRECTION.Up * velocity;
		}
		if (direction == DOWN)
		{
			m_Transform.GetPositionRef() -= WORLD_DIRECTION.Up * velocity;
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
		XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_Transform.GetRotation().x, m_Transform.GetRotation().y, 0.0f);
		XMVECTOR camTarget = XMVector3TransformCoord(WORLD_DIRECTION.Forward, camRotationMatrix);
		camTarget += m_Transform.GetPosition();
		XMVECTOR upDir = XMVector3TransformCoord(WORLD_DIRECTION.Up, camRotationMatrix);
		m_ViewMatrix = XMMatrixLookAtLH(m_Transform.GetPosition(), camTarget, upDir);

		m_Transform.UpdateLocalDirectionVectors();
	}
}