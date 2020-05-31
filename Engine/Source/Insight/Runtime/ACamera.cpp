#include <ie_pch.h>


#include "ACamera.h"
#include "imgui.h"
#include "Insight/Runtime/Components/Actor_Component.h"

namespace Insight {



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

	void ACamera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
	{
		m_Fov = fovDegrees;
		m_NearZ = nearZ;
		m_FarZ = farZ;
		m_AspectRatio = aspectRatio;
		float fovRadians = fovDegrees * (3.14f / 180.0f);
		m_ProjectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, m_AspectRatio, m_NearZ, m_FarZ);
	}

	void ACamera::RenderSceneHeirarchy()
	{
		AActor::RenderSceneHeirarchy();
	}

	void ACamera::OnImGuiRender()
	{
		ImGui::Text("View");
		ImGui::DragFloat("Field of View", &m_Fov, 0.5f, 0.0f, 180.0f);
		ImGui::DragFloat("Near Z", &m_NearZ, 0.01f, 0.00001f, 5.0f);
		ImGui::DragFloat("Far Z", &m_FarZ, 1.0f, 1.0f, 10000.0f);

		ImGui::Text("Post-Processing");
		ImGui::DragFloat("Exposure", &m_Exposure, 0.01f, 0.0f, 1.0f);

		SetProjectionValues(m_Fov, m_AspectRatio, m_NearZ, m_FarZ);
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