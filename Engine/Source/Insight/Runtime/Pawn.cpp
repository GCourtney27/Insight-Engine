#include "ie_pch.h"

#include "Pawn.h"

namespace Insight {



	Pawn::Pawn()
	{
	}

	Pawn::~Pawn()
	{
	}

	void Pawn::OnInit()
	{
	}

	void Pawn::OnUpdate()
	{
	}

	void Pawn::OnRender()
	{
	}

	void Pawn::Move(eMovement direction, const float& deltaTime)
	{
		float velocity = m_MovementSpeed * deltaTime;

		switch (direction)
		{
		case Insight::Pawn::FORWARD:
		{
			m_Transform.GetPositionRef() += m_Transform.GetLocalForward() * velocity;
			break;
		}
		case Insight::Pawn::BACKWARD:
		{
			m_Transform.GetPositionRef() -= m_Transform.GetLocalForward() * velocity;
			break;
		}
		case Insight::Pawn::LEFT:
		{
			m_Transform.GetPositionRef() -= m_Transform.GetLocalRight() * velocity;
			break;
		}
		case Insight::Pawn::RIGHT:
		{
			m_Transform.GetPositionRef() += m_Transform.GetLocalRight() * velocity;
			break;
		}
		case Insight::Pawn::UP:
		{
			m_Transform.GetPositionRef() += m_Transform.GetLocalUp() * velocity;
			break;
		}
		case Insight::Pawn::DOWN:
		{
			m_Transform.GetPositionRef() -= m_Transform.GetLocalUp() * velocity;
			break;
		}
		default:
			break;
		}
	}


}

