#pragma once

#include <Runtime/Core.h>

#include "Runtime/GameFramework/AActor.h"
#include "Runtime/GameFramework/Components/InputComponent.h"


namespace Insight {

	namespace GameFramework {


		constexpr float DEFAULT_BASE_SPEED	= 60.05f;
		constexpr float DEFAULT_BOOST_SPEED = DEFAULT_BASE_SPEED * 7.0f;


		class INSIGHT_API APawn : public AActor
		{
		public:
			using Super = AActor;
		public:
			APawn(ActorId id, ActorName name = "Pawn");
			virtual ~APawn();

			virtual bool OnInit() override;
			virtual void OnUpdate(const float DeltaMs) override;
			virtual void OnRender() override;

			inline void SetMovementSpeed(const float& movementSpeed) { m_MovementSpeed = movementSpeed; }

		protected:
			void Move(const ieVector3& Direction, const float Value)
			{
				float Velocity = m_MovementSpeed * Value * m_DeltaMs;
				m_pSceneComponent->GetPositionRef() += Direction * Velocity;
			}
			void MoveForward(float Value);
			void MoveRight(float Value);
			void MoveUp(float Value);
			void Sprint();

		protected:
			float m_MovementSpeed;
			bool m_Sprinting;

			InputComponent* m_pInputComponent;
			SceneComponent* m_pSceneComponent;
		};

	} // end namespace GameFramework
} // end namespace Insight
