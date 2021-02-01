#pragma once

#include <Runtime/CoreMacros.h>

#include "Runtime/GameFramework/Archetypes/APawn.h"


namespace Insight {

	using namespace DirectX::SimpleMath;

	namespace GameFramework {


		constexpr float DEFAULT_SENSITIVITY = 150.0f;
		constexpr float DEFAULT_FOV			= 45.0f;
		constexpr float DEFAULT_EXPOSURE	= 0.5f;
		constexpr float DEFAULT_NEAR_Z		= 0.5f;
		constexpr float DEFAULT_FAR_Z		= 3000.0f;

		using namespace DirectX;

		// Represents the outline of a camera. There can only be one
		// camera in the world at any given time. To switch to a 
		// 'new camera' set the view target of the global camera.
		struct ViewTarget
		{
			ieVector3 Position	= Vector3::Zero;
			ieVector3 Rotation	= Vector3::Zero;
			float FieldOfView	= DEFAULT_FOV;
			float Sensitivity	= DEFAULT_SENSITIVITY;
			float Speed			= DEFAULT_BASE_SPEED;
			float Exposure		= DEFAULT_EXPOSURE;
			float NearZ			= DEFAULT_NEAR_Z;
			float FarZ			= DEFAULT_FAR_Z;
		};


		class INSIGHT_API ACamera : public APawn
		{
		public:
			friend class APlayerCharacter;
			using Super = APawn;

		public:
			ACamera(ViewTarget ViewTarget);
			virtual ~ACamera();

			virtual void BeginPlay() override;
			virtual void OnUpdate(const float DeltaMs) override;
			virtual void EditorEndPlay() override;

			void ProcessMouseScroll(float yOffset);
			void ProcessMouseMovement(float xOffset, float yOffset);

			ieVector3 GetPosition() const { return m_pSceneComponent->GetPosition(); }
			inline XMMATRIX GetViewMatrix() const { return m_ViewMatrix; }
			inline XMMATRIX GetProjectionMatrix() const { return m_ProjectionMatrix; };
			inline float GetFOV() const { return m_Fov; }
			inline float GetNearZ() const { return m_NearZ; }
			inline float GetFarZ() const { return m_FarZ; }
			inline float GetExposure() { return m_Exposure; }
			inline void SetFOV(float fovDegrees) { SetPerspectiveProjectionValues(fovDegrees, m_AspectRatio, m_NearZ, m_FarZ); }
			inline void SetNearZ(float nearZ) { SetPerspectiveProjectionValues(m_Fov, m_AspectRatio, nearZ, m_FarZ); }
			inline void SetFarZ(float farZ) { SetPerspectiveProjectionValues(m_Fov, m_AspectRatio, m_NearZ, farZ); }
			inline void SetExposure(float exposure) { m_Exposure = exposure; }

			static ViewTarget GetDefaultViewTarget() { return ViewTarget(); }
			inline void SetViewTarget(ViewTarget& ViewTarget, bool UpdateProjection = true, bool UpdateView = true)
			{
				m_pSceneComponent->SetPosition(ViewTarget.Position);
				m_pSceneComponent->SetRotation(ViewTarget.Rotation);

				m_Fov = ViewTarget.FieldOfView;
				m_MouseSensitivity = ViewTarget.Sensitivity;
				m_MovementSpeed = ViewTarget.Speed;
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
				m_pSceneComponent->GetTransformRef().UpdateLocalDirectionVectors();
			}

			void OnEvent(Event& e);

			void SetPerspectiveProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
			void SetOrthographicsProjectionValues(float viewWidth, float viewHeight, float nearZ, float farZ);
			void RenderSceneHeirarchy() override;
			void OnImGuiRender() override;

			// Returns truw if the camera is currently in a orthographics projection state. Returns false if not.
			int GetIsOrthographic() { return m_IsOrthographic; }
		private:
			void UpdateViewMatrix();
			bool OnMouseScrolled(MouseScrolledEvent& e);

			// TODO: Move this to the Pawn class! Translation
			void MoveForward(float Value);
			void MoveRight(float Value);
			void MoveUp(float Value);
			void LookUp(float Value);
			void LookRight(float Value);
			void TogglePitchYawRotation();
			void Sprint();

			// DEBUG
			void Test();
		private:
			XMFLOAT4X4 m_ViewMat4x4;
			XMMATRIX m_ViewMatrix;
			XMFLOAT4X4 m_ProjectionMat4x4;
			XMMATRIX m_ProjectionMatrix;

			bool m_IsOrthographic = false;
			bool CanRotateCamera = false;
			float m_Yaw = 0.0f;
			float m_Pitch = 0.0f;
			float m_Roll = 0.0f;

			float m_MouseSensitivity = 0.0f;

			float m_Fov = 0.0f;
			float m_NearZ = 0.0001f;
			float m_FarZ = 0.0f;
			float m_AspectRatio = 0.0f;
			float m_Exposure = 1.0f;

			// Cached Internal Variables.
			XMMATRIX m_CamRotationMatrix;
			XMVECTOR m_CamTarget;
			XMVECTOR m_UpDir;

		};

	} // end namespace GameFramework
} // end namesapce Insight
