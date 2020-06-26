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
		m_ViewTarget = ACamera::GetDefaultViewTarget(); // This should be loaded through a player settings file

		m_pCamera = &ACamera::Get();
	}

	APlayerCharacter::~APlayerCharacter()
	{
		m_pCamera = nullptr;
	}

	bool APlayerCharacter::OnInit()
	{
		APawn::OnInit();

		return true;
	}

	void APlayerCharacter::OnUpdate(const float& deltaMs)
	{
		APawn::OnUpdate(deltaMs);
	}

	void APlayerCharacter::OnPreRender(XMMATRIX parentMat)
	{
		APawn::OnPreRender(parentMat);
	}

	void APlayerCharacter::OnRender()
	{
		APawn::OnRender();
	}

	void APlayerCharacter::Tick(const float& DeltaMs)
	{
		ProcessInput(DeltaMs);
	}

	void APlayerCharacter::RenderSceneHeirarchy()
	{
		AActor::RenderSceneHeirarchy();
	}

	void APlayerCharacter::OnImGuiRender()
	{
		AActor::OnImGuiRender();
	}

	void APlayerCharacter::ProcessInput(const float& deltaMs)
	{
		if (Input::IsMouseButtonPressed(IE_MOUSEBUTTON_RIGHT))
		{
			auto [x, y] = Input::GetRawMousePosition();
			m_pCamera->ProcessMouseMovement((float)x, (float)y);
			if (Input::IsKeyPressed('W'))
			{
				APawn::Move(eMovement::FORWARD, deltaMs);
				m_pCamera->ProcessKeyboardInput(CameraMovement::FORWARD, deltaMs);
			}
			if (Input::IsKeyPressed('S'))
			{
				APawn::Move(eMovement::BACKWARD, deltaMs);
				m_pCamera->ProcessKeyboardInput(CameraMovement::BACKWARD, deltaMs);
			}
			if (Input::IsKeyPressed('A'))
			{
				APawn::Move(eMovement::LEFT, deltaMs);
				m_pCamera->ProcessKeyboardInput(CameraMovement::LEFT, deltaMs);
			}
			if (Input::IsKeyPressed('D'))
			{
				APawn::Move(eMovement::RIGHT, deltaMs);
				m_pCamera->ProcessKeyboardInput(CameraMovement::RIGHT, deltaMs);
			}
			if (Input::IsKeyPressed('E'))
			{
				APawn::Move(eMovement::UP, deltaMs);
				m_pCamera->ProcessKeyboardInput(CameraMovement::UP, deltaMs);
			}
			if (Input::IsKeyPressed('Q'))
			{
				APawn::Move(eMovement::DOWN, deltaMs);
				m_pCamera->ProcessKeyboardInput(CameraMovement::DOWN, deltaMs);
			}
		}

	}

}
