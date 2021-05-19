#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/ECS/EntityAdmin/EntityAdminFwd.h"


namespace Insight
{
	class ieWorld;

	class INSIGHT_API ieObjectBase
	{
	private:
		/*
			Add the object to the world.
		*/
		void AddToWorld();

		/*
			Remove the actor from the world and destory any components it posesses.
		*/
		void RemoveFromWorld();

	protected:
		ieObjectBase(ieWorld* pWorld, const FString& Name = TEXT("Unnamed ieObject"));
		virtual ~ieObjectBase();

		inline ieWorld* GetWorld() const
		{
			return m_pWorld;
		}

		inline ECS::Entity_t GetWorldId() const 
		{
			return m_WorldId;
		}

		inline void PushComponentId(const ECS::ComponentUID_t& ComponentId)
		{
			m_ComponentIds.push_back(ComponentId);
		}

	private:
		/*
			Cached world pointer.
		*/
		ieWorld* m_pWorld;

		/*
			The name of this object.
		*/
		FString m_Name;

		/*
			The tag of this object.
		*/
		FString m_Tag;

		/*
			The id of this Actor's placement inside the entity admin.
		*/
		ECS::Entity_t m_WorldId;

		/*
			Cached component ids.
		*/
		std::vector<ECS::ComponentUID_t> m_ComponentIds;
	};
}
