#pragma once

#include "Insight/Core.h"

#include "Insight/Runtime/Actor.h"


namespace Insight {

	class INSIGHT_API Pawn : public Actor
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
		Pawn();
		~Pawn();

		virtual void OnInit();
		virtual void OnUpdate();
		virtual void OnRender();

		inline void SetMovementSpeed(const float& movementSpeed) { m_MovementSpeed = movementSpeed; }

	protected:
		void Move(eMovement direction, const float& deltaTime);
	private:
		float m_MovementSpeed = 20.0f;
	};
}
