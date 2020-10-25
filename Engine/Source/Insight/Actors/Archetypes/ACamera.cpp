#include <Engine_pch.h>

#include "ACamera.h"

#include "Insight/Actors/Components/Actor_Component.h"
#include "Insight/Actors/Components/Input_Component.h"
#include "Insight/Actors/Archetypes/APlayer_Character.h"
#include "Insight/Core/Application.h"
#include "Insight/Input/Input.h"
#include "Insight/Input/Key_Codes.h"

#include "imgui.h"

namespace Insight {

	namespace Runtime {



		ACamera::ACamera(ViewTarget ViewTarget)
			: AActor(0, "Camera")
		{
			m_pSceneComponent = CreateDefaultSubobject<SceneComponent>();
			m_pInputComponent = CreateDefaultSubobject<InputComponent>();
			
			// Setup event callbacks for camera movement.
			m_pInputComponent->BindAxis("MoveForward", IE_BIND_EVENT_FN(ACamera::MoveForward));
			m_pInputComponent->BindAxis("MoveRight", IE_BIND_EVENT_FN(ACamera::MoveRight));
			m_pInputComponent->BindAxis("MoveUp", IE_BIND_EVENT_FN(ACamera::MoveUp));
			m_pInputComponent->BindAxis("LookUp", IE_BIND_EVENT_FN(ACamera::LookUp));
			m_pInputComponent->BindAxis("LookRight", IE_BIND_EVENT_FN(ACamera::LookRight));
			
			m_pInputComponent->BindAction("MouseButtonPress", InputEventType_Pressed, IE_BIND_VOID_FN(ACamera::ButtonPressTest));
			m_pInputComponent->BindAction("MouseButtonPress", InputEventType_Released, IE_BIND_VOID_FN(ACamera::ButtonReleaseTest));


			SetViewTarget(ViewTarget, false, true);
		}

		ACamera::~ACamera()
		{
		}

		void ACamera::BeginPlay()
		{
		}
		static float s_DeltaMs = 0.0f;
		void ACamera::OnUpdate(const float DeltaMs)
		{
			s_DeltaMs = DeltaMs;
		}

		void ACamera::EditorEndPlay()
		{

		}

		void ACamera::ProcessMouseScroll(float yOffset)
		{
		}

		void ACamera::ProcessMouseMovement(float xPos, float yPos)
		{
			m_pSceneComponent->Rotate(yPos * m_MouseSensitivity, xPos * m_MouseSensitivity, 0.0f);

			UpdateViewMatrix();
			m_pSceneComponent->GetTransformRef().UpdateLocalDirectionVectors();
		}

		void ACamera::OnEvent(Event& e)
		{
			EventDispatcher Dispatcher(e);
			Dispatcher.Dispatch<MouseScrolledEvent>(IE_BIND_EVENT_FN(ACamera::OnMouseScrolled));

		}

		void ACamera::SetPerspectiveProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
		{
			m_Fov = fovDegrees;
			m_NearZ = nearZ;
			m_FarZ = farZ;
			m_AspectRatio = aspectRatio;
			float fovRadians = fovDegrees * (3.14f / 180.0f);
			m_ProjectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, m_AspectRatio, m_NearZ, m_FarZ);
		}

		void ACamera::SetOrthographicsProjectionValues(float viewWidth, float viewHeight, float nearZ, float farZ)
		{
			m_ProjectionMatrix = XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
		}

		void ACamera::RenderSceneHeirarchy()
		{
			AActor::RenderSceneHeirarchy();
		}

		void ACamera::OnImGuiRender()
		{
			AActor::OnImGuiRender();

			ImGui::Text("View");
			if (!m_IsOrthographic) {
				ImGui::DragFloat("Field of View", &m_Fov, 0.5f, 1.0f, 180.0f);
			}
			ImGui::DragFloat("Near Z", &m_NearZ, 0.01f, 0.00001f, 5.0f);
			ImGui::DragFloat("Far Z", &m_FarZ, 1.0f, 1.0f, 10000.0f);

			ImGui::Text("Post-Processing");
			ImGui::DragFloat("Exposure", &m_Exposure, 0.01f, 0.0f, 1.0f);

			ImGui::Text("Projection");
			constexpr char* projectionMethods[] = { "Perspective", "Orthographic" };
			ImGui::Combo("Method", (int*)&m_IsOrthographic, projectionMethods, IM_ARRAYSIZE(projectionMethods));
			if (!m_IsOrthographic) {
				SetPerspectiveProjectionValues(m_Fov, m_AspectRatio, m_NearZ, m_FarZ);
			}
			else {
				SetOrthographicsProjectionValues(40.0f, 40.0f, m_NearZ, m_FarZ);
			}

		}

		void ACamera::UpdateViewMatrix()
		{
			m_CamRotationMatrix = XMMatrixRotationRollPitchYaw(m_pSceneComponent->GetRotation().x, m_pSceneComponent->GetRotation().y, 0.0f);
			m_CamTarget = XMVector3TransformCoord(Vector3::Forward, m_CamRotationMatrix);
			m_CamTarget += m_pSceneComponent->GetPosition();
			m_UpDir = XMVector3TransformCoord(Vector3::Up, m_CamRotationMatrix);
			m_ViewMatrix = XMMatrixLookAtLH(m_pSceneComponent->GetPosition(), m_CamTarget, m_UpDir);
		}

		bool ACamera::OnMouseScrolled(MouseScrolledEvent& e)
		{
			if (Application::Get().GetImGuiLayer().IsMouseOverUI()) {
				return false;
			}

			return false;
		}

		void ACamera::MoveForward(float Value)
		{
			float Velocity = m_MovementSpeed * Value * s_DeltaMs;
			ieVector3 Direction = m_pSceneComponent->GetTransform().GetLocalForward() * Velocity;
			m_pSceneComponent->GetPositionRef() += Direction;
			UpdateViewMatrix();
		}

		void ACamera::MoveRight(float Value)
		{
			float Velocity = m_MovementSpeed * Value * s_DeltaMs;
			ieVector3 Direction = m_pSceneComponent->GetTransform().GetLocalRight() * Velocity;
			m_pSceneComponent->GetPositionRef() += Direction;
			UpdateViewMatrix();
		}

		void ACamera::MoveUp(float Value)
		{
			float Velocity = m_MovementSpeed * Value * s_DeltaMs;
			ieVector3 Direction = m_pSceneComponent->GetTransform().GetLocalUp() * Velocity;
			m_pSceneComponent->GetPositionRef() += Direction;
			UpdateViewMatrix();
		}

		void ACamera::LookUp(float Value)
		{
			//if (CanRotateCamera)
			{
				m_pSceneComponent->Rotate(Value * m_MouseSensitivity * s_DeltaMs, 0.0f, 0.0f);

				UpdateViewMatrix();
				m_pSceneComponent->GetTransformRef().UpdateLocalDirectionVectors();
			}
		}

		void ACamera::LookRight(float Value)
		{
			//if (CanRotateCamera)
			{
				m_pSceneComponent->Rotate(0.0f, Value * m_MouseSensitivity * s_DeltaMs, 0.0f);

				UpdateViewMatrix();
				m_pSceneComponent->GetTransformRef().UpdateLocalDirectionVectors();
			}
		}

		void ACamera::UnlockCamRotation()
		{
			IE_CORE_INFO("Funciton called");
			CanRotateCamera = !CanRotateCamera;
		}

		void ACamera::ButtonPressTest()
		{
			IE_CORE_INFO("Pressed");
		}

		void ACamera::ButtonReleaseTest()
		{
			IE_CORE_INFO("Released");
		}

	} // end namespace Runtime
} // end namespace Insight