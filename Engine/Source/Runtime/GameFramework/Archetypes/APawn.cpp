#include <Engine_pch.h>

#include "APawn.h"

#include "Core/Public/Events/MouseEvent.h"

namespace Insight {

	namespace GameFramework {


		APawn::APawn(ActorId id, ActorName name)
			: AActor(id, name), m_MovementSpeed(DEFAULT_BASE_SPEED), m_Sprinting(false)
		{
			m_pSceneComponent = CreateDefaultSubobject<SceneComponent>();
			m_pInputComponent = CreateDefaultSubobject<InputComponent>();
		}

		APawn::~APawn()
		{

		}

		bool APawn::OnInit()
		{
			Super::OnInit();

			return true;
		}

		void APawn::OnUpdate(const float DeltaMs)
		{
			Super::OnUpdate(DeltaMs);

		}

		void APawn::OnRender()
		{
			Super::OnRender();
		}

		void APawn::MoveForward(float Value)
		{
			Move(m_pSceneComponent->GetTransform().GetLocalForward(), Value);
		}

		void APawn::MoveRight(float Value)
		{
			Move(m_pSceneComponent->GetTransform().GetLocalRight(), Value);
		}

		void APawn::MoveUp(float Value)
		{
			Move(m_pSceneComponent->GetTransform().GetLocalUp(), Value);
		}

		void APawn::Sprint()
		{
			m_Sprinting = !m_Sprinting;
			if (m_Sprinting)
			{
				m_MovementSpeed = DEFAULT_BASE_SPEED;
			}
			else
			{
				m_MovementSpeed = DEFAULT_BOOST_SPEED;
			}
		}

	} // end namespace GameFramework
} // end namespace Insight
