// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/ieObject/ieObjectBase.h"


namespace Insight
{
	class ieWorld;

	class INSIGHT_API ieActor : public ieObjectBase
	{
	public:
		using Super = ieActor;

		ieActor(ieWorld* pWorld, const FString& Name = L"Unamed ieActor");
		virtual ~ieActor()
		{
		}
		
		virtual void BeginPlay()
		{
		}

		virtual void Tick(float DeltaMs)
		{
		}

		inline bool CanReceiveTickEvents() const;

		inline bool CanBeTicked(bool Value);


		template <typename SubObjectType, typename ... Args>
		FORCE_INLINE SubObjectType& CreateDefaultSubObject(const FString& Name, Args ... args)
		{
			SubObjectType& pResult = GetWorld()->GetEntityAdmin().AddComponent<SubObjectType>(GetWorldId(), args...);
			PushComponentId(pResult.GetId());
			pResult.SetOwner(this);
			return pResult;
		}

		template <typename SubObjectType>
		FORCE_INLINE void RemoveSubObjectById(const ECS::ComponentUID_t& ComponentId)
		{
			GetWorld()->GetEntityAdmin().RemoveComponentById<SubObjectType>(ComponentId);
		}

		template <typename SubObjectType>
		FORCE_INLINE void RemoveSubObject(const SubObjectType& Component)
		{
			GetWorld()->GetEntityAdmin().RemoveComponentById<SubObjectType>(Component.GetId());
		}

	protected:
		bool m_bCanReveiveTickEvents;
	};


	//
	// Inline function implementations
	//

	inline bool ieActor::CanReceiveTickEvents() const 
	{
		return m_bCanReveiveTickEvents;
	}

	inline bool ieActor::CanBeTicked(bool Value)
	{
		m_bCanReveiveTickEvents = Value;
	}

}
