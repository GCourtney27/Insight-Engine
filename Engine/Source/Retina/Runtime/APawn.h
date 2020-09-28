#pragma once

#include <Retina/Core.h>

#include "Retina/Runtime/AActor.h"


namespace Retina {

	namespace Runtime {


		class RETINA_API APawn : public AActor
		{
		public:
			enum eMovement
			{
				FORWARD,
				BACKWARD,
				LEFT,
				RIGHT,
				UP,
				DOWN
			};
		public:
			APawn(ActorId id, ActorName name = "Pawn");
			virtual ~APawn();

			virtual bool OnInit() override;
			virtual void OnUpdate(const float DeltaMs) override;
			virtual void OnRender() override;

			inline void SetMovementSpeed(const float& movementSpeed) { m_MovementSpeed = movementSpeed; }

		protected:
			void Move(eMovement direction, const float& deltaTime);
		private:
			float m_MovementSpeed = 20.0f;
			SceneComponent* m_pSceneComponent = nullptr;
		};

	} // end namespace Runtime
} // end namespace Retina
