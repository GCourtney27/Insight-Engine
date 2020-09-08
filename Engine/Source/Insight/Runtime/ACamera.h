#pragma once

#include <Insight/Core.h>

#include "Insight/Runtime/AActor.h"

#include "Insight/Events/Mouse_Event.h"

namespace Insight {

	using namespace DirectX::SimpleMath;

	namespace Runtime {

		enum class CameraMovement
		{
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT,
			UP,
			DOWN
		};

		const float BASE_SPEED = 30.05f;
		const float BOOST_SPEED = 100.05f;
		const float SENSITIVITY = 0.2f;
		const float FOV = 45.0f;
		const float EXPOSURE = 0.5f;
		const float NEAR_Z = 0.0001f;
		const float FAR_Z = 1000.0f;

		using namespace DirectX;

		// Represents the outline of a camera. There can only be one
		// camera in the world at any given time. To switch to a 
		// 'new camera' set the view target of the global camera.
		struct ViewTarget
		{
			ieVector3 Position = Vector3::Zero;
			ieVector3 Rotation = Vector3::Zero;
			float FieldOfView = FOV;
			float Sensitivity = SENSITIVITY;
			float Speed = BASE_SPEED;
			float Exposure = EXPOSURE;
			float NearZ = NEAR_Z;
			float FarZ = FAR_Z;
		};

		class INSIGHT_API ACamera : public AActor
		{
		public:
			friend class APlayerCharacter;
		public:
			ACamera(ViewTarget ViewTarget);
			virtual ~ACamera();

			inline static ACamera& Get() { return *s_Instance; }

			virtual void BeginPlay() override;
			virtual void OnUpdate(const float DeltaMs) override;
			virtual void EditorEndPlay() override;

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

			static ViewTarget GetDefaultViewTarget() { return ViewTarget{ ieVector3{0.0f, 10.0f, -20.0f}, ieVector3{0.0f, 0.0f, 0.0f}, FOV, SENSITIVITY, BASE_SPEED, EXPOSURE, NEAR_Z, FAR_Z }; }

			inline void SetViewTarget(ViewTarget& ViewTarget, bool UpdateProjection = true, bool UpdateView = true)
			{
				GetTransformRef().SetPosition(ViewTarget.Position);
				GetTransformRef().SetRotation(ViewTarget.Rotation);

				m_Fov = ViewTarget.FieldOfView;
				m_MouseSensitivity = ViewTarget.Sensitivity;
				m_MovementSpeed = ViewTarget.Speed;
				m_BaseMovementSpeed = ViewTarget.Speed;
				m_Exposure = ViewTarget.Exposure;
				m_NearZ = ViewTarget.NearZ;
				m_FarZ = ViewTarget.FarZ;

				if (UpdateProjection) {
					SetPerspectiveProjectionValues(
						m_Fov,
						m_AspectRatio,
						m_NearZ,
						m_FarZ
					);
				}

				if (UpdateView) {
					UpdateViewMatrix();
				}
				GetTransformRef().UpdateLocalDirectionVectors();
			}

			void OnEvent(Event& e);

			void SetPerspectiveProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
			void SetOrthographicsProjectionValues(float viewWidth, float viewHeight, float nearZ, float farZ);
			void RenderSceneHeirarchy() override;
			void OnImGuiRender() override;

			int GetIsOrthographic() { return m_IsOrthographic; }

		private:
			void UpdateViewMatrix();
			bool OnMouseScrolled(MouseScrolledEvent& e);
		private:

			XMFLOAT4X4 m_ViewMat4x4;
			XMMATRIX m_ViewMatrix;
			XMFLOAT4X4 m_ProjectionMat4x4;
			XMMATRIX m_ProjectionMatrix;
			bool m_IsOrthographic = false;

			float m_Yaw = 0.0f;
			float m_Pitch = 0.0f;
			float m_Roll = 0.0f;

			float m_MovementSpeed = 0.0f;
			float m_BaseMovementSpeed = 0.0f;
			float m_MouseSensitivity = 0.0f;

			float m_Fov = 0.0f;
			float m_NearZ = 0.0001f;
			float m_FarZ = 0.0f;
			float m_AspectRatio = 0.0f;

			float m_Exposure = 1.0f;
		private:
			static ACamera* s_Instance;
		};

	} // end namespace Runtime
} // end namesapce Insight
