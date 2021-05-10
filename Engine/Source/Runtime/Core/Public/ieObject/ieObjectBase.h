#pragma once

#include <Runtime/Core.h>

namespace Insight
{
	class INSIGHT_API ieObjectBase
	{
	protected:
		ieObjectBase(const FString& Name = TEXT("Unnamed Actor"))
			: m_Name(Name)
		{
		}
		virtual ~ieObjectBase()
		{
		}

		FString m_Name;
	};
}
