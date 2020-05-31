#pragma once

#include <Insight/Core.h>

#include "Insight/Runtime/AActor.h"

namespace Insight {

	using namespace DirectX::SimpleMath;

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
	const float SPEED = 30.05f;
	const float SENSITIVITY = 0.005f;
	const float FOV = 45.0f;
	const float EXPOSURE = 0.5f;

	using namespace DirectX;

	class INSIGHT_API ACamera : public AActor
	{
	public:
		friend class APlayerCharacter;
	public:
		ACamera(Vector3 position = Vector3(0.0f, 10.0f, -10.0f),
			float pitch = PITCH,
			float yaw = YAW,
			float roll = ROLL,
			float exposure = EXPOSURE)
			: m_MovementSpeed(SPEED), m_MouseSensitivity(SENSITIVITY), m_Fov(FOV), m_Exposure(EXPOSURE), AActor(0)
		{
			AActor(0, "Camera");
			GetTransformRef().SetPosition(position);
			m_Pitch = pitch;
			m_Yaw = yaw;
			m_Roll = roll;
			UpdateViewMatrix();
		}
		virtual ~ACamera();

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
		inline float GetExposure() { return m_Exposure; }
		inline void SetExposure(float exposure) { m_Exposure = exposure; }

		void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
		void RenderSceneHeirarchy() override;

	private:
		void UpdateViewMatrix();
	private:

		XMFLOAT4X4 m_ViewMat4x4;
		XMMATRIX m_ViewMatrix;
		XMFLOAT4X4 m_ProjectionMat4x4;
		XMMATRIX m_ProjectionMatrix;

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
