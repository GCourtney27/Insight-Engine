#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/ieObject/ieObjectBase.h"

namespace Insight
{

	template <typename DerivedType>
	class INSIGHT_API ieComponentBase : public ECS::ComponentBase<DerivedType>
	{
		friend class ieObjectBase;
	protected:
		ieComponentBase()
			: m_pOwner(NULL)
		{
		}
		virtual ~ieComponentBase()
		{
		}

		inline void SetOwner(ieObjectBase* pOwner)
		{
			m_pOwner = pOwner;
		}

	private:
		ieObjectBase* m_pOwner;
	};
}
