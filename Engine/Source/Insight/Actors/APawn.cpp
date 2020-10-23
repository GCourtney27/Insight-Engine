#include <Engine_pch.h>

#include "APawn.h"

namespace Insight {

	namespace Runtime {


		APawn::APawn(ActorId id, ActorName name)
			: AActor(id, name)
		{
			m_pSceneComponent = CreateDefaultSubobject<SceneComponent>();
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
				m_pSceneComponent->GetPositionRef() += m_pSceneComponent->GetTransformRef().GetLocalForward() * velocity;
				break;
			}
			case Runtime::APawn::BACKWARD:
			{
				m_pSceneComponent->GetPositionRef() -= m_pSceneComponent->GetTransformRef().GetLocalForward() * velocity;
				break;
			}
			case Runtime::APawn::LEFT:
			{
				m_pSceneComponent->GetPositionRef() -= m_pSceneComponent->GetTransformRef().GetLocalRight() * velocity;
				break;
			}
			case Runtime::APawn::RIGHT:
			{
				m_pSceneComponent->GetPositionRef() += m_pSceneComponent->GetTransformRef().GetLocalRight() * velocity;
				break;
			}
			case Runtime::APawn::UP:
			{
				m_pSceneComponent->GetPositionRef() += m_pSceneComponent->GetTransformRef().GetLocalUp() * velocity;
				break;
			}
			case Runtime::APawn::DOWN:
			{
				m_pSceneComponent->GetPositionRef() -= m_pSceneComponent->GetTransformRef().GetLocalUp() * velocity;
				break;
			}
			default:
				IE_CORE_INFO("Failed to determine direction to move pawn: {0}", SceneNode::GetDisplayName());
				break;
			}
		}

	} // end namespace Runtime
} // end namespace Insight
