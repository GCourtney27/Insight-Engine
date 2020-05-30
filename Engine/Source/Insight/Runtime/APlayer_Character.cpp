#include <ie_pch.h>

#include "APlayer_Character.h"

#include "Insight/Core/Application.h"
#include "Insight/Input/Input.h"
#include "imgui.h"
#include "Insight/Runtime/Components/Actor_Component.h"

namespace Insight {

	APlayerCharacter* APlayerCharacter::s_Instance = nullptr;

	APlayerCharacter::APlayerCharacter(ActorId id, ActorName name)
		: APawn(id, name)
	{
		IE_CORE_ASSERT(!s_Instance, "Trying to create another instnace of a player character!");
		s_Instance = this;

		float windowWidth = (float)Application::Get().GetWindow().GetWidth();
		float windowHeight = (float)Application::Get().GetWindow().GetHeight();

		m_Camera = new ACamera();
		m_Camera->SetDisplayName("Player Camera");
		m_Camera->SetProjectionValues(75.0f, windowWidth / windowHeight, 0.0001f, 100.0f);
		SceneNode::AddChild(m_Camera);
	}

	APlayerCharacter::~APlayerCharacter()
	{
	}

	bool APlayerCharacter::OnInit()
	{
		APawn::OnInit();

		return true;
	}

	void APlayerCharacter::OnUpdate(const float& deltaMs)
	{
		APawn::OnUpdate(deltaMs);
		ProcessInput(deltaMs);
	}

	void APlayerCharacter::OnPreRender(XMMATRIX parentMat)
	{
		APawn::OnPreRender(parentMat);
	}

	void APlayerCharacter::OnRender()
	{
		APawn::OnRender();
	}

	void APlayerCharacter::RenderSceneHeirarchy()
	{
		if (ImGui::TreeNode(SceneNode::GetDisplayName()))
		{
			SceneNode::RenderSceneHeirarchy();

			for (size_t i = 0; i < m_NumComponents; ++i)
			{
				m_Components[i]->RenderSceneHeirarchy();
			}
			ImGui::TreePop();
			ImGui::Spacing();
		}
	}

	void APlayerCharacter::ProcessInput(const float& deltaMs)
	{
		if (Input::IsMouseButtonPressed(IE_MOUSEBUTTON_RIGHT))
		{
			auto [x, y] = Input::GetRawMousePosition();
			m_Camera->ProcessMouseMovement((float)x, (float)y);
		}
		if (Input::IsKeyPressed('W'))
		{
			APawn::Move(eMovement::FORWARD, deltaMs);
			m_Camera->ProcessKeyboardInput(CameraMovement::FORWARD, deltaMs);
		}
		if (Input::IsKeyPressed('S'))
		{
			APawn::Move(eMovement::BACKWARD, deltaMs);
			m_Camera->ProcessKeyboardInput(CameraMovement::BACKWARD, deltaMs);
		}
		if (Input::IsKeyPressed('A'))
		{
			APawn::Move(eMovement::LEFT, deltaMs);
			m_Camera->ProcessKeyboardInput(CameraMovement::LEFT, deltaMs);
		}
		if (Input::IsKeyPressed('D'))
		{
			APawn::Move(eMovement::RIGHT, deltaMs);
			m_Camera->ProcessKeyboardInput(CameraMovement::RIGHT, deltaMs);
		}
		if (Input::IsKeyPressed('E'))
		{
			APawn::Move(eMovement::UP, deltaMs);
			m_Camera->ProcessKeyboardInput(CameraMovement::UP, deltaMs);
		}
		if (Input::IsKeyPressed('Q'))
		{
			APawn::Move(eMovement::DOWN, deltaMs);
			m_Camera->ProcessKeyboardInput(CameraMovement::DOWN, deltaMs);
		}
	}

}
