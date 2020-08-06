#include <ie_pch.h>

#include "APawn.h"

namespace Insight {



	APawn::APawn(ActorId id, ActorName name)
		: AActor(id, name)
	{
	}

	APawn::~APawn()
	{

	}

	bool APawn::OnInit()
	{
		AActor::OnInit();

		return true;
	}

	void APawn::OnUpdate(const float DeltaMs)
	{
		AActor::OnUpdate(DeltaMs);

	}

	void APawn::CalculateParent(XMMATRIX parentMat)
	{
		AActor::CalculateParent(parentMat);
	}

	void APawn::OnRender()
	{
		AActor::OnRender();
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
			IE_CORE_INFO("Failed to determine direction to move pawn: {0}", SceneNode::GetDisplayName());
			break;
		}
	}


}

