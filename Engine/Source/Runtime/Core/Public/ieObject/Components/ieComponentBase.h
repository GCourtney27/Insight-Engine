#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/ieObject/ieObjectBase.h"

namespace Insight
{

	template <typename DerivedType>
	class INSIGHT_API ieComponentBase : public ECS::ComponentBase<DerivedType>
	{
		friend class ieActor;
		friend class ieObjectBase;
	public:
		using Super = ieComponentBase;

	protected:
		ieComponentBase()
			: m_pOwner(NULL)
		{
		}
		virtual ~ieComponentBase()
		{
		}

		virtual void BeginPlay()
		{
		}

		virtual void Tick(float DeltaMs)
		{
		}

		inline void SetOwner(ieObjectBase* pOwner)
		{
			m_pOwner = pOwner;
		}

		inline const FString& GetName() const
		{
			return m_Name;
		}

		inline void SetName(const FString& Name)
		{
			m_Name = Name;
		}

	private:
		ieObjectBase* m_pOwner;
		FString m_Name;
	};
}
