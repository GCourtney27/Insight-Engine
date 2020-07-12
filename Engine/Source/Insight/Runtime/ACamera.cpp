#include <ie_pch.h>


#include "ACamera.h"
#include "imgui.h"
#include "Insight/Runtime/Components/Actor_Component.h"
#include "Insight/Runtime/APlayer_Character.h"
#include "Insight/Core/Application.h"
#include "Insight/Input/Input.h"


namespace Insight {

	ACamera* ACamera::s_Instance = nullptr;

	ACamera::ACamera(Vector3 position, float pitch, float yaw, float roll, float exposure)
		: m_MovementSpeed(SPEED), m_MouseSensitivity(SENSITIVITY), m_Fov(FOV), m_Exposure(EXPOSURE), AActor(0, "Camera")
	{
		IE_CORE_ASSERT(!s_Instance, "Cannot have more than one camera in the world at once!");
		s_Instance = this;

		GetTransformRef().SetPosition(position);
		m_Pitch = pitch;
		m_Yaw = yaw;
		m_Roll = roll;
		UpdateViewMatrix();
	}

	ACamera::ACamera(ViewTarget ViewTarget)
		: AActor(0, "Camera")
	{
		IE_CORE_ASSERT(!s_Instance, "Cannot have more than one camera in the world at once!");
		s_Instance = this;

		GetTransformRef().SetPosition(ViewTarget.Position);
		m_Fov = ViewTarget.FieldOfView;
		m_MouseSensitivity = ViewTarget.Sensitivity;
		m_MovementSpeed = ViewTarget.Speed;
		m_Exposure = ViewTarget.Exposure;
		m_NearZ = ViewTarget.NearZ;
		m_FarZ = ViewTarget.FarZ;

		UpdateViewMatrix();
	}

	ACamera::~ACamera()
	{
	}

	void ACamera::BeginPlay()
	{
		
	}

	// TODO Strip this out for game distribution we dont need it 
	// during runtime character controller will do this for us
	void ACamera::OnUpdate(const float& DeltaMs)
	{
		if (Application::Get().IsPlaySessionUnderWay()) {
			return;
		}

		if (Input::IsMouseButtonPressed(IE_MOUSEBUTTON_RIGHT))
		{
			auto [x, y] = Input::GetRawMousePosition();
			ProcessMouseMovement((float)x, (float)y);
			if (Input::IsKeyPressed('W'))
			{
				ProcessKeyboardInput(CameraMovement::FORWARD, DeltaMs);
			}
			if (Input::IsKeyPressed('S'))
			{
				ProcessKeyboardInput(CameraMovement::BACKWARD, DeltaMs);
			}
			if (Input::IsKeyPressed('A'))
			{
				ProcessKeyboardInput(CameraMovement::LEFT, DeltaMs);
			}
			if (Input::IsKeyPressed('D'))
			{
				ProcessKeyboardInput(CameraMovement::RIGHT, DeltaMs);
			}
			if (Input::IsKeyPressed('E'))
			{
				ProcessKeyboardInput(CameraMovement::UP, DeltaMs);
			}
			if (Input::IsKeyPressed('Q'))
			{
				ProcessKeyboardInput(CameraMovement::DOWN, DeltaMs);
			}
		}
	}

	void ACamera::EditorEndPlay()
	{

	}

	void ACamera::ProcessMouseScroll(float yOffset)
	{
	}

	void ACamera::ProcessMouseMovement(float xPos, float yPos)
	{
		GetTransformRef().Rotate(yPos * m_MouseSensitivity, xPos * m_MouseSensitivity, 0.0f);

		UpdateViewMatrix();
		GetTransformRef().UpdateLocalDirectionVectors();
	}

	void ACamera::ProcessKeyboardInput(CameraMovement direction, float deltaTime)
	{
		float velocity = m_MovementSpeed * deltaTime;
		if (direction == CameraMovement::FORWARD)
		{
			GetTransformRef().GetPositionRef() += GetTransformRef().GetLocalForward() * velocity;
		}
		if (direction == CameraMovement::BACKWARD)
		{
			GetTransformRef().GetPositionRef() -= GetTransformRef().GetLocalForward() * velocity;
		}
		if (direction == CameraMovement::LEFT)
		{
			GetTransformRef().GetPositionRef() -= GetTransformRef().GetLocalRight() * velocity;
		}
		if (direction == CameraMovement::RIGHT)
		{
			GetTransformRef().GetPositionRef() += GetTransformRef().GetLocalRight() * velocity;
		}
		if (direction == CameraMovement::UP)
		{
			GetTransformRef().GetPositionRef() += WORLD_DIRECTION.Up * velocity;
		}
		if (direction == CameraMovement::DOWN)
		{
			GetTransformRef().GetPositionRef() -= WORLD_DIRECTION.Up * velocity;
		}
		UpdateViewMatrix();
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
		//m_ProjectionMatrix = XMMatrixOrthographicOffCenterLH(0.0f, viewWidth, 0.0f, viewHeight, nearZ, farZ);
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
		ImGui::Combo("Method", &m_IsOrthographic, projectionMethods, IM_ARRAYSIZE(projectionMethods));
		if (!m_IsOrthographic) {
			SetPerspectiveProjectionValues(m_Fov, m_AspectRatio, m_NearZ, m_FarZ);
		}
		else {
			SetOrthographicsProjectionValues(40.0f, 40.0f, m_NearZ, m_FarZ);
		}

	}

	void ACamera::UpdateViewMatrix()
	{
		XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(GetTransformRef().GetRotation().x, GetTransformRef().GetRotation().y, 0.0f);
		XMVECTOR camTarget = XMVector3TransformCoord(WORLD_DIRECTION.Forward, camRotationMatrix);
		camTarget += GetTransformRef().GetPosition();
		XMVECTOR upDir = XMVector3TransformCoord(WORLD_DIRECTION.Up, camRotationMatrix);
		m_ViewMatrix = XMMatrixLookAtLH(GetTransformRef().GetPosition(), camTarget, upDir);
	}
}