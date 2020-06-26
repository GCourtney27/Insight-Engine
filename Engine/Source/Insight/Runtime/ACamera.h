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
	const float SENSITIVITY = 0.2f;
	const float FOV = 45.0f;
	const float EXPOSURE = 0.5f;
	const float NEAR_Z = 0.0001f;
	const float FAR_Z = 1000.0f;

	using namespace DirectX;

	struct ViewTarget
	{
		Vector3 Position;
		Vector3 Rotation;
		float FieldOfView;
		float Sensitivity;
		float Speed;
		float Exposure;
		float NearZ;
		float FarZ;
	};

	class INSIGHT_API ACamera : public AActor
	{
	public:
		friend class APlayerCharacter;
	public:
		ACamera(Vector3 position = Vector3(0.0f, 10.0f, -20.0f),
			float pitch = PITCH,
			float yaw = YAW,
			float roll = ROLL,
			float exposure = EXPOSURE);
			
		ACamera(ViewTarget ViewTarget);
		virtual ~ACamera();

		inline static ACamera& Get() { return *s_Instance; }

		void ProcessMouseScroll(float yOffset);
		void ProcessMouseMovement(float xOffset, float yOffset);
		void ProcessKeyboardInput(CameraMovement direction, float deltaTime);

		const inline XMMATRIX& GetViewMatrix() const { return m_ViewMatrix; }
		const inline XMMATRIX& GetProjectionMatrix() const { return m_ProjectionMatrix; };
		const inline float& GetFOV() const { return m_Fov; }
		const inline float& GetNearZ() const { return m_NearZ; }
		const inline float& GetFarZ() const { return m_FarZ; }
		inline void SetFOV(float& fovDegrees) { SetPerspectiveProjectionValues(fovDegrees, m_AspectRatio, m_NearZ, m_FarZ); }
		inline void SetNearZ(float& nearZ) { SetPerspectiveProjectionValues(m_Fov, m_AspectRatio, nearZ, m_FarZ); }
		inline void SetFarZ(float& farZ) { SetPerspectiveProjectionValues(m_Fov, m_AspectRatio, m_NearZ, farZ); }
		inline float GetExposure() { return m_Exposure; }
		inline void SetExposure(float exposure) { m_Exposure = exposure; }

		static ViewTarget GetDefaultViewTarget() { return ViewTarget{ Vector3{0.0f, 10.0f, -20.0f}, Vector3{0.0f, 0.0f, 0.0f}, FOV, SENSITIVITY, SPEED, EXPOSURE, NEAR_Z, FAR_Z }; }

		inline void SetViewTarget(ViewTarget& ViewTarget) 
		{ 
			GetTransformRef().SetPosition(ViewTarget.Position); 
			GetTransformRef().SetRotation(ViewTarget.Rotation); 

			m_Fov = ViewTarget.FieldOfView; 
			m_MouseSensitivity = ViewTarget.Sensitivity;
			m_MovementSpeed = ViewTarget.Speed;
			m_Exposure = ViewTarget.Exposure; 
			m_NearZ = ViewTarget.NearZ; 
			m_FarZ = ViewTarget.FarZ; 

			SetPerspectiveProjectionValues(
				m_Fov,
				m_AspectRatio,
				m_NearZ,
				m_FarZ
			);

			UpdateViewMatrix();
			GetTransformRef().UpdateLocalDirectionVectors();
		}

		void SetPerspectiveProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
		void SetOrthographicsProjectionValues(float viewWidth, float viewHeight, float nearZ, float farZ);
		void RenderSceneHeirarchy() override;
		void OnImGuiRender() override;

		BOOL GetIsOrthographic() { return m_IsOrthographic; }

	private:
		void UpdateViewMatrix();
	private:

		XMFLOAT4X4 m_ViewMat4x4;
		XMMATRIX m_ViewMatrix;
		XMFLOAT4X4 m_ProjectionMat4x4;
		XMMATRIX m_ProjectionMatrix;
		BOOL m_IsOrthographic = false;

		float m_Yaw = 0.0f;
		float m_Pitch = 0.0f;
		float m_Roll = 0.0f;

		float m_MovementSpeed = 0.0f;
		float m_MouseSensitivity = 0.0f;

		float m_Fov = 0.0f;
		float m_NearZ = 0.0001f;
		float m_FarZ = 0.0f;
		float m_AspectRatio = 0.0f;

		float m_Exposure = 1.0f;
	private:
		static ACamera* s_Instance;
	};

}
