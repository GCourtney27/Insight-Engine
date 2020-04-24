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
	const float ROLL = 0.0f;
	const float SPEED = 4.05f;
	const float SENSITIVITY = 0.01f;
	const float FOV = 45.0f;
	const float EXPOSURE = 0.5f;

	using namespace DirectX;

	class INSIGHT_API Camera
	{
	public:
		Camera(XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f), float pitch = PITCH, float yaw = YAW, float roll = ROLL, float exposure = EXPOSURE)
			: m_Front(XMFLOAT3(0.0f, 0.0f, 1.0f)), m_MovementSpeed(SPEED), m_MouseSensitivity(SENSITIVITY), m_Fov(FOV), m_Exposure(EXPOSURE)
		{
			m_Position = position;
			m_PositionVector = XMLoadFloat3(&m_Position);
			m_WorldUp = up;
			m_WorldUpVector = XMLoadFloat3(&m_WorldUp);
			m_Pitch = pitch;
			m_Yaw = yaw;
			m_Roll = roll;
			m_Exposure = exposure;
			UpdateViewMatrix();
		}
		~Camera();

		void ProcessMouseScroll(float yOffset);
		void ProcessMouseMovement(float xOffset, float yOffset);
		void ProcessKeyboardInput(CameraMovement direction, float deltaTime);

		const inline XMMATRIX& GetViewMatrix() const { return m_ViewMatrix; }
		const inline XMMATRIX& GetProjectionMatrix() const { return m_ProjectionMatrix; };
		const inline float& GetFOV() const { return m_Fov; }
		const inline float& GetNearZ() const { return m_NearZ; }
		const inline float& GetFarZ() const { return m_FarZ; }
		inline void SetFOV(float& fovDegrees) { SetProjectionValues(fovDegrees, m_AspectRatio, m_NearZ, m_FarZ); }
		inline void SetNearZ(float& nearZ) { SetProjectionValues(m_Fov, m_AspectRatio, nearZ, m_FarZ); }
		inline void SetFarZ(float& farZ) { SetProjectionValues(m_Fov, m_AspectRatio, m_NearZ, farZ); }

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
		XMFLOAT3 m_Down;
		XMVECTOR m_DownVector;
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
		bool m_ConstrainPitch = true;

		float m_Fov = 0.0f;
		float m_NearZ = 0.0001f;
		float m_FarZ = 0.0f;
		float m_AspectRatio = 0.0f;

		float m_Exposure = 1.0f;

		bool m_FirstMove = true;
		float m_LastLookX = 0.0f;
		float m_LastLookY = 0.0f;
	};

}
