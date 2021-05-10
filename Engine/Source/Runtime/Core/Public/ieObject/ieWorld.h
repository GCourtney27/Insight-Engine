#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/ieObject/ieObjectBase.h"

#include "Runtime/Core/Public/ECS/ECS.h"

namespace Insight
{
	class INSIGHT_API ieWorld : public ieObjectBase
	{
	public:
		ieWorld() 
		{
		
		}
		~ieWorld() 
		{
			m_EntityAdmin.Flush();
		}

		ECS::Entity_t CreateActor() 
		{
			return m_EntityAdmin.CreateEntity();
		}

		void DestroyActor(ECS::Entity_t Actor)
		{
			m_EntityAdmin.DestroyEntity(Actor);
		}

	private:
		ECS::EntityAdmin m_EntityAdmin;
	};
}