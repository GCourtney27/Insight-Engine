#include <ie_pch.h>

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

	void APawn::OnPreRender(XMMATRIX parentMat)
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
			GetTransformRef().GetPositionRef() += GetTransformRef().GetLocalForward() * velocity;
			break;
		}
		case Insight::APawn::BACKWARD:
		{
			GetTransformRef().GetPositionRef() -= GetTransformRef().GetLocalForward() * velocity;
			break;
		}
		case Insight::APawn::LEFT:
		{
			GetTransformRef().GetPositionRef() -= GetTransformRef().GetLocalRight() * velocity;
			break;
		}
		case Insight::APawn::RIGHT:
		{
			GetTransformRef().GetPositionRef() += GetTransformRef().GetLocalRight() * velocity;
			break;
		}
		case Insight::APawn::UP:
		{
			GetTransformRef().GetPositionRef() += GetTransformRef().GetLocalUp() * velocity;
			break;
		}
		case Insight::APawn::DOWN:
		{
			GetTransformRef().GetPositionRef() -= GetTransformRef().GetLocalUp() * velocity;
			break;
		}
		default:
			break;
		}
	}


}

