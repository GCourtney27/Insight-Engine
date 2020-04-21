#include "ie_pch.h"

#include "Camera.h"
#include "Insight/Math/Transform.h"


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

		UpdateViewMatrix();
	}
	
	void Camera::ProcessKeyboardInput(CameraMovement direction, float deltaTime)
	{
		float velocity = m_MovementSpeed * deltaTime;
		if (direction == FORWARD)
		{
			m_PositionVector += m_FrontVector * velocity;
			XMStoreFloat3(&m_Position, m_PositionVector);
		}
		if (direction == BACKWARD)
		{
			m_PositionVector -= m_FrontVector * velocity;
			XMStoreFloat3(&m_Position, m_PositionVector);
		}
		if (direction == LEFT)
		{
			m_PositionVector -= m_RightVector * velocity;
			XMStoreFloat3(&m_Position, m_PositionVector);
		}
		if (direction == RIGHT)
		{
			m_PositionVector += m_RightVector * velocity;
			XMStoreFloat3(&m_Position, m_PositionVector);
		}
		if (direction == UP)
		{
			m_PositionVector += m_UpVector * velocity;
			XMStoreFloat3(&m_Position, m_PositionVector);
		}
		if (direction == DOWN)
		{
			m_PositionVector -= m_UpVector * velocity;
			XMStoreFloat3(&m_Position, m_PositionVector);
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

	void Camera::UpdateLocalVectors()
	{
		XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, m_Roll);
		m_FrontVector = XMVector3TransformCoord(Vector3_WorldForward,  vecRotationMatrix);
		m_BackVector  = XMVector3TransformCoord(Vector3_WorldBackward, vecRotationMatrix);
		m_LeftVector  = XMVector3TransformCoord(Vector3_WorldLeft,	   vecRotationMatrix);
		m_RightVector = XMVector3TransformCoord(Vector3_WorldRight,	   vecRotationMatrix);
		m_UpVector	  = XMVector3TransformCoord(Vector3_WorldUp,	   vecRotationMatrix);
		m_DownVector  = XMVector3TransformCoord(Vector3_WorldDown,	   vecRotationMatrix);
	}

	void Camera::UpdateViewMatrix()
	{
		XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, m_Roll);
		XMVECTOR camTarget = XMVector3TransformCoord(Vector3_WorldForward, camRotationMatrix);
		camTarget += m_PositionVector;
		XMVECTOR upDir = XMVector3TransformCoord(Vector3_WorldUp, camRotationMatrix);
		m_ViewMatrix = XMMatrixLookAtLH(m_PositionVector, camTarget, upDir);

		UpdateLocalVectors();
	}
}