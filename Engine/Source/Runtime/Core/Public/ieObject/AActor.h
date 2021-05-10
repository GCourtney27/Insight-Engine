#pragma once

#include "Runtime/Core/Public/ieObject/ieObjectBase.h"

#include "Runtime/Core/Public/ieObject/ieWorld.h"


namespace Insight
{
	class INSIGHT_API AActor : public ieObjectBase
	{
	public:
		AActor(ieWorld* pWorld) 
			: m_pWorld(pWorld)
		{
			IE_ASSERT(pWorld != NULL);

			AddToWorld();
		}
		virtual ~AActor() 
		{
			RemoveFromWorld();
		}
		

	private:
		void AddToWorld()
		{
			m_WorldId = m_pWorld->CreateActor();
		}

		void RemoveFromWorld()
		{
			m_pWorld->DestroyActor(m_WorldId);
		}

	protected:
		ECS::Entity_t m_WorldId;
		ieWorld* m_pWorld;
	};
}
