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

	protected:
		bool m_bCanReveiveTickEvents;
	};

	//
	// INline function implementations
	//

	inline bool ieActor::CanReceiveTickEvents() const 
	{
		return m_bCanReveiveTickEvents;
	}

}
