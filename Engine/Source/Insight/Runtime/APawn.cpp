#include "ie_pch.h"

#include "APawn.h"

namespace Insight {



	APawn::APawn(ActorId id)
		: AActor(id)
	{
	}

	APawn::~APawn()
	{

	}

	bool APawn::OnInit()
	{

		return true;
	}

	void APawn::OnUpdate(const float& deltaMs)
	{
	}

	void APawn::OnRender()
	{
	}

	void APawn::Move(eMovement direction, const float& deltaTime)
	{
		float velocity = m_MovementSpeed * deltaTime;

		switch (direction)
		{
		case Insight::APawn::FORWARD:
		{
			m_Transform.GetPositionRef() += m_Transform.GetLocalForward() * velocity;
			break;
		}
		case Insight::APawn::BACKWARD:
		{
			m_Transform.GetPositionRef() -= m_Transform.GetLocalForward() * velocity;
			break;
		}
		case Insight::APawn::LEFT:
		{
			m_Transform.GetPositionRef() -= m_Transform.GetLocalRight() * velocity;
			break;
		}
		case Insight::APawn::RIGHT:
		{
			m_Transform.GetPositionRef() += m_Transform.GetLocalRight() * velocity;
			break;
		}
		case Insight::APawn::UP:
		{
			m_Transform.GetPositionRef() += m_Transform.GetLocalUp() * velocity;
			break;
		}
		case Insight::APawn::DOWN:
		{
			m_Transform.GetPositionRef() -= m_Transform.GetLocalUp() * velocity;
			break;
		}
		default:
			break;
		}
	}


}

