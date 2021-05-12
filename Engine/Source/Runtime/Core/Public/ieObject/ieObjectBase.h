#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/ECS/EntityAdmin/EntityAdminFwd.h"


namespace Insight
{
	class ieWorld;

	class INSIGHT_API ieObjectBase
	{
	public:
		template <typename SubObjectType, typename ... Args>
		FORCE_INLINE SubObjectType& CreateDefaultSubObject(const FString& Name, Args ... args)
		{
			SubObjectType* pResult = m_pWorld->m_EntityAdmin.AddComponent<SubObjectType>(m_WorldId, args...);
			m_ComponentIds.push_back(pResult->GetId());
			pResult->SetOwner(this);
			return *pResult;
		}

		template <typename SubObjectType>
		FORCE_INLINE void RemoveSubObjectById(const ECS::ComponentUID_t& ComponentId)
		{
			m_pWorld->m_EntityAdmin.RemoveComponentById<SubObjectType>(ComponentId);
		}

	private:
		void AddToWorld();

		void RemoveFromWorld();

	protected:
		ieObjectBase(ieWorld* pWorld, const FString& Name = TEXT("Unnamed ieObject"));
		virtual ~ieObjectBase();


		ieWorld* m_pWorld;
		FString m_Name;
		FString m_Tag;
		ECS::Entity_t m_WorldId;

		std::vector<ECS::ComponentUID_t> m_ComponentIds;
	};
}
