#include "ie_pch.h"

#include "APlayer_Character.h"

#include "Insight/Core/Application.h"
#include "Insight/Input/Input.h"


namespace Insight {

	APlayerCharacter* APlayerCharacter::s_Instance = nullptr;

	APlayerCharacter::APlayerCharacter()
	{
		IE_CORE_ASSERT(!s_Instance, "Trying to create another instnace of a player character!");
		s_Instance = this;

		float windowWidth = (float)Application::Get().GetWindow().GetWidth();
		float windowHeight = (float)Application::Get().GetWindow().GetHeight();

		m_Camera.SetProjectionValues(75.0f, windowWidth / windowHeight, 0.0001f, 100.0f);
	}

	APlayerCharacter::~APlayerCharacter()
	{
	}

	void APlayerCharacter::OnInit()
	{
	}

	void APlayerCharacter::OnUpdate()
	{

	}

	void APlayerCharacter::OnRender()
	{
	}

	void APlayerCharacter::ProcessInput()
	{
		// this should move the player and the camera moves along with it
		if (Input::IsKeyPressed('W'))
		{
			APawn::Move(eMovement::DOWN, 0.001f);
			//Set Camera transform relative to player
			m_Camera.ProcessKeyboardInput(CameraMovement::FORWARD, 0.001f);
		}
		if (Input::IsKeyPressed('S'))
		{

			m_Camera.ProcessKeyboardInput(CameraMovement::BACKWARD, 0.001f);
		}
		if (Input::IsKeyPressed('A'))
		{

			m_Camera.ProcessKeyboardInput(CameraMovement::LEFT, 0.001f);
		}
		if (Input::IsKeyPressed('D'))
		{

			m_Camera.ProcessKeyboardInput(CameraMovement::RIGHT, 0.001f);
		}
		if (Input::IsKeyPressed('E'))
		{

			m_Camera.ProcessKeyboardInput(CameraMovement::UP, 0.001f);
		}
		if (Input::IsKeyPressed('Q'))
		{

			m_Camera.ProcessKeyboardInput(CameraMovement::DOWN, 0.001f);
		}

	}

}
