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

	void Camera::ProcessMouseMovement(float xOffset, float yOffset)
	{
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

		float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
		m_ProjectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
	}

	void Camera::UpdateLocalVectors()
	{
		XMVECTOR WorldForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		XMVECTOR WorldBackward = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		XMVECTOR WorldLeft = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR WorldRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR WorldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR WorldDown = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);

		XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, m_Roll);
		m_FrontVector = XMVector3TransformCoord(WorldForward, vecRotationMatrix);
		m_BackVector = XMVector3TransformCoord(WorldBackward, vecRotationMatrix);
		m_LeftVector = XMVector3TransformCoord(WorldLeft, vecRotationMatrix);
		m_RightVector = XMVector3TransformCoord(WorldRight, vecRotationMatrix);
	}

	void Camera::UpdateViewMatrix()
	{
		XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, m_Roll);
		XMVECTOR camTarget = XMVector3TransformCoord(WorldForward, camRotationMatrix);
		camTarget += m_PositionVector;
		XMVECTOR upDir = XMVector3TransformCoord(WorldUp, camRotationMatrix);
		m_ViewMatrix = XMMatrixLookAtLH(m_PositionVector, camTarget, upDir);

		UpdateLocalVectors();
	}
}