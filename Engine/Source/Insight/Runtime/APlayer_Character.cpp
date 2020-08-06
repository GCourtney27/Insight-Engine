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

	void APlayerCharacter::OnUpdate(const float DeltaMs)
	{
		APawn::OnUpdate(DeltaMs);
	}

	void APlayerCharacter::CalculateParent(XMMATRIX parentMat)
	{
		APawn::CalculateParent(parentMat);
	}

	void APlayerCharacter::OnRender()
	{
		APawn::OnRender();
	}

	void APlayerCharacter::Tick(const float DeltaMs)
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

	void APlayerCharacter::ProcessInput(const float DeltaMs)
	{
		if (Input::IsMouseButtonPressed(IE_MOUSEBUTTON_RIGHT))
		{
			auto [x, y] = Input::GetRawMousePosition();
			m_pCamera->ProcessMouseMovement((float)x, (float)y);
			if (Input::IsKeyPressed('W'))
			{
				APawn::Move(eMovement::FORWARD, DeltaMs);
				m_pCamera->ProcessKeyboardInput(CameraMovement::FORWARD, DeltaMs);
			}
			if (Input::IsKeyPressed('S'))
			{
				APawn::Move(eMovement::BACKWARD, DeltaMs);
				m_pCamera->ProcessKeyboardInput(CameraMovement::BACKWARD, DeltaMs);
			}
			if (Input::IsKeyPressed('A'))
			{
				APawn::Move(eMovement::LEFT, DeltaMs);
				m_pCamera->ProcessKeyboardInput(CameraMovement::LEFT, DeltaMs);
			}
			if (Input::IsKeyPressed('D'))
			{
				APawn::Move(eMovement::RIGHT, DeltaMs);
				m_pCamera->ProcessKeyboardInput(CameraMovement::RIGHT, DeltaMs);
			}
			if (Input::IsKeyPressed('E'))
			{
				APawn::Move(eMovement::UP, DeltaMs);
				m_pCamera->ProcessKeyboardInput(CameraMovement::UP, DeltaMs);
			}
			if (Input::IsKeyPressed('Q'))
			{
				APawn::Move(eMovement::DOWN, DeltaMs);
				m_pCamera->ProcessKeyboardInput(CameraMovement::DOWN, DeltaMs);
			}
		}

	}

}
