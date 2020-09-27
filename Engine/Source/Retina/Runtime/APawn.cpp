#include <Engine_pch.h>

#include "APawn.h"

namespace Retina {

	namespace Runtime {


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
			case Runtime::APawn::FORWARD:
			{
				GetTransformRef().GetPositionRef() += GetTransformRef().GetLocalForward() * velocity;
				break;
			}
			case Runtime::APawn::BACKWARD:
			{
				GetTransformRef().GetPositionRef() -= GetTransformRef().GetLocalForward() * velocity;
				break;
			}
			case Runtime::APawn::LEFT:
			{
				GetTransformRef().GetPositionRef() -= GetTransformRef().GetLocalRight() * velocity;
				break;
			}
			case Runtime::APawn::RIGHT:
			{
				GetTransformRef().GetPositionRef() += GetTransformRef().GetLocalRight() * velocity;
				break;
			}
			case Runtime::APawn::UP:
			{
				GetTransformRef().GetPositionRef() += GetTransformRef().GetLocalUp() * velocity;
				break;
			}
			case Runtime::APawn::DOWN:
			{
				GetTransformRef().GetPositionRef() -= GetTransformRef().GetLocalUp() * velocity;
				break;
			}
			default:
				RN_CORE_INFO("Failed to determine direction to move pawn: {0}", SceneNode::GetDisplayName());
				break;
			}
		}

	} // end namespace Runtime
} // end namespace Retina
