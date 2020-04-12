#pragma once

#include "Insight/Core.h"

#include <DirectXMath.h>

namespace Insight {

	enum CameraMovement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 2.05f;
	const float SENSITIVITY = 0.1f;
	const float FOV = 45.0f;
	const float EXPOSURE = 0.5f;

	using namespace DirectX;

	class INSIGHT_API Camera
	{
	public:
		Camera(XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float exposure = EXPOSURE)
			: m_Front(XMFLOAT3(0.0f, 0.0f, 1.0f)), m_MovementSpeed(SPEED), m_MouseSensitivity(SENSITIVITY), m_Fov(FOV), m_Exposure(EXPOSURE)
		{
			m_Position = position;
			m_PositionVector = XMLoadFloat3(&m_Position);
			m_WorldUp = up;
			m_WorldUpVector = XMLoadFloat3(&m_WorldUp);
			m_Yaw = yaw;
			m_Pitch = pitch;
			m_Exposure = exposure;
			UpdateViewMatrix();
		}
		~Camera();

		void ProcessMouseScroll(float yOffset);
		void ProcessMouseMovement(float xOffset, float yOffset);
		void ProcessKeyboardInput(CameraMovement direction, float deltaTime);

		const inline XMMATRIX& GetViewMatrix() const { return m_ViewMatrix; }
		const inline XMMATRIX& GetProjectionMatrix() const { return m_ProjectionMatrix; };

		void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	private:
		void UpdateLocalVectors();
		void UpdateViewMatrix();
	private:
		XMFLOAT4X4 m_ViewMat4x4;
		XMMATRIX m_ViewMatrix;

		XMFLOAT4X4 m_ProjectionMat4x4;
		XMMATRIX m_ProjectionMatrix;

		XMFLOAT3 m_Position;
		XMVECTOR m_PositionVector;

		XMFLOAT3 m_Front;
		XMVECTOR m_FrontVector;
		
		XMFLOAT3 m_Back;
		XMVECTOR m_BackVector;

		XMFLOAT3 m_Up;
		XMVECTOR m_UpVector;

		XMFLOAT3 m_Right;
		XMVECTOR m_RightVector;

		XMFLOAT3 m_Left;
		XMVECTOR m_LeftVector;

		XMFLOAT3 m_WorldUp;
		XMVECTOR m_WorldUpVector;

		float m_Yaw = 0.0f;
		float m_Pitch = 0.0f;
		float m_Roll = 0.0f;

		float m_MovementSpeed = 0.0f;
		float m_MouseSensitivity = 0.0f;
		float m_Fov = 0.0f;

		float m_Exposure = 1.0f;


	};

}
