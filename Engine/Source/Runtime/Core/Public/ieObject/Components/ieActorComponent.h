#pragma once

#include "EngineDefines.h"

#include "Core/Public/ieObject/Components/ieComponentBase.h"


namespace Insight
{
	class ieActor;


	class INSIGHT_API ieActorComponent : public ieComponentBase
	{
	public:
		ieActorComponent(ieActor* pOwner)
			: m_pOwner(pOwner)
		{
			IE_ASSERT(m_pOwner != NULL);
		}
		~ieActorComponent() = default;


		inline ieActor* GetOwner() const;

	private:
		ieActor* m_pOwner;
	};

	//
	// Inline Function Implementations
	//

	inline ieActor* ieActorComponent::GetOwner() const
	{
		return m_pOwner;
	}
}
