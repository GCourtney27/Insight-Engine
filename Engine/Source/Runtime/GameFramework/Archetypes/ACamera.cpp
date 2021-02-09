// Copyright Insight Interactive. All Rights Reserved.
#include <Engine_pch.h>

#include "ACamera.h"

#include "Runtime/GameFramework/Components/ActorComponent.h"
#include "Runtime/Core/Application.h"

#include "Runtime/UI/UILib.h"


namespace Insight {

	namespace GameFramework {



		ACamera::ACamera(ViewTarget ViewTarget)
			: APawn(0, "Camera")
		{
			SetViewTarget(ViewTarget, false, true);

			// Setup event callbacks for camera movement.
			m_pInputComponent->BindAxis("MoveForward", IE_BIND_LOCAL_EVENT_FN(ACamera::MoveForward));
			m_pInputComponent->BindAxis("MoveRight", IE_BIND_LOCAL_EVENT_FN(ACamera::MoveRight));
			m_pInputComponent->BindAxis("MoveUp", IE_BIND_LOCAL_EVENT_FN(ACamera::MoveUp));
			m_pInputComponent->BindAxis("LookUp", IE_BIND_LOCAL_EVENT_FN(ACamera::LookUp));
			m_pInputComponent->BindAxis("LookRight", IE_BIND_LOCAL_EVENT_FN(ACamera::LookRight));
			m_pInputComponent->BindAxis("MouseWheelUp", IE_BIND_LOCAL_EVENT_FN(ACamera::MoveForward));

			m_pInputComponent->BindAction("CameraPitchYawLock", IET_Pressed, IE_BIND_LOCAL_VOID_FN(ACamera::TogglePitchYawRotation));
			m_pInputComponent->BindAction("CameraPitchYawLock", IET_Released, IE_BIND_LOCAL_VOID_FN(ACamera::TogglePitchYawRotation));
			m_pInputComponent->BindAction("Sprint", IET_Pressed, IE_BIND_LOCAL_VOID_FN(ACamera::Sprint));
			m_pInputComponent->BindAction("Sprint", IET_Released, IE_BIND_LOCAL_VOID_FN(ACamera::Sprint));
		}

		ACamera::~ACamera()
		{
		}

		void ACamera::BeginPlay()
		{
		}

		void ACamera::OnUpdate(const float DeltaMs)
		{
			Super::OnUpdate(DeltaMs);
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
			Dispatcher.Dispatch<MouseScrolledEvent>(IE_BIND_LOCAL_EVENT_FN(ACamera::OnMouseScrolled));

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
			Super::RenderSceneHeirarchy();
		}

		void ACamera::OnImGuiRender()
		{
			Super::OnImGuiRender();

			UI::Text("View");
			if (!m_IsOrthographic) {
				UI::DragFloat("Field of View", &m_Fov, 0.5f, 1.0f, 180.0f);
			}
			UI::DragFloat("Near Z", &m_NearZ, 0.01f, 0.00001f, 5.0f);
			UI::DragFloat("Far Z", &m_FarZ, 1.0f, 1.0f, 10000.0f);

			UI::Text("Post-Processing");
			UI::DragFloat("Exposure", &m_Exposure, 0.01f, 0.0f, 1.0f);

			UI::Text("Projection");
			constexpr char* projectionMethods[] = { "Perspective", "Orthographic" };
			UI::ComboBox("Method", *((int*)&m_IsOrthographic), projectionMethods, _countof(projectionMethods));
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
			Super::MoveForward(Value);
			UpdateViewMatrix();
		}

		void ACamera::MoveRight(float Value)
		{
			Super::MoveRight(Value);
			UpdateViewMatrix();
		}

		void ACamera::MoveUp(float Value)
		{
			Super::MoveUp(Value);
			UpdateViewMatrix();
		}

		void ACamera::LookUp(float Value)
		{
			if (CanRotateCamera)
			{
				m_pSceneComponent->Rotate(Value * m_MouseSensitivity * m_DeltaMs, 0.0f, 0.0f);

				UpdateViewMatrix();
				m_pSceneComponent->GetTransformRef().UpdateLocalDirectionVectors();
			}
		}

		void ACamera::LookRight(float Value)
		{
			if (CanRotateCamera)
			{
				m_pSceneComponent->Rotate(0.0f, Value * m_MouseSensitivity * m_DeltaMs, 0.0f);

				UpdateViewMatrix();
				m_pSceneComponent->GetTransformRef().UpdateLocalDirectionVectors();
			}
		}

		void ACamera::TogglePitchYawRotation()
		{
			CanRotateCamera = !CanRotateCamera;
		}

		void ACamera::Sprint()
		{
			m_Sprinting = !m_Sprinting;
		
			if (m_Sprinting)
				m_MovementSpeed = DEFAULT_BOOST_SPEED;
			else
				m_MovementSpeed = DEFAULT_BASE_SPEED;
		}

		void ACamera::Test()
		{
			IE_LOG(Log, "Test Fn!");
		}


	} // end namespace GameFramework
} // end namespace Insight