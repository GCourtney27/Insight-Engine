// Copyright Insight Interactive. All Rights Reserved.
#include <Engine_pch.h>

#include "APlayerCharacter.h"

#include "Core/Public/Engine.h"
#include "GameFramework/Components/ActorComponent.h"

namespace Insight {

	namespace GameFramework {


		APlayerCharacter* APlayerCharacter::s_Instance = nullptr;

		APlayerCharacter::APlayerCharacter(ActorId id, ActorName name)
			: APawn(id, name)
		{
			IE_ASSERT(!s_Instance, "Trying to create another instnace of a player character!");
			s_Instance = this;
			m_ViewTarget = ACamera::GetDefaultViewTarget(); // This should be loaded through a player settings file

			//m_pCamera = new ACamera(m_ViewTarget);

			m_pCharacterRoot = CreateDefaultSubobject<SceneComponent>();
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
			/*if (Input::IsMouseButtonPressed(MouseButton::MouseButton_Right))
			{
				auto [x, y] = Input::GetRawMousePosition();
				m_pCamera->ProcessMouseMovement((float)x, (float)y);
				
			}*/

		}

	} // end namespace GameFramework
} // end namespace Insight
