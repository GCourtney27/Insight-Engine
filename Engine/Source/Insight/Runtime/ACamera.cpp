#include <ie_pch.h>


#include "ACamera.h"
#include "imgui.h"
#include "Insight/Runtime/Components/Actor_Component.h"
#include "Insight/Core/Application.h"

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
		if (direction == FORWARD)
		{
			GetTransformRef().GetPositionRef() += GetTransformRef().GetLocalForward() * velocity;
		}
		if (direction == BACKWARD)
		{
			GetTransformRef().GetPositionRef() -= GetTransformRef().GetLocalForward() * velocity;
		}
		if (direction == LEFT)
		{
			GetTransformRef().GetPositionRef() -= GetTransformRef().GetLocalRight() * velocity;
		}
		if (direction == RIGHT)
		{
			GetTransformRef().GetPositionRef() += GetTransformRef().GetLocalRight() * velocity;
		}
		if (direction == UP)
		{
			GetTransformRef().GetPositionRef() += WORLD_DIRECTION.Up * velocity;
		}
		if (direction == DOWN)
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
		static const char* projectionMethods[] = { "Perspective", "Orthographic" };
		ImGui::Combo("Method", &m_IsOrthographic, projectionMethods, IM_ARRAYSIZE(projectionMethods));
		if (!m_IsOrthographic) {
			SetPerspectiveProjectionValues(m_Fov, m_AspectRatio, m_NearZ, m_FarZ);
		}
		else {
			SetOrthographicsProjectionValues((float)Application::Get().GetWindow().GetWidth(), (float)Application::Get().GetWindow().GetHeight(), m_NearZ, m_FarZ);
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