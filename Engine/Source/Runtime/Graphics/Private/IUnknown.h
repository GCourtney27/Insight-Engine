#pragma once

#include <Runtime/Core.h>
#include <Runtime/Graphics/Public/GraphicsCore.h>

namespace Insight
{
	namespace Graphics
	{
		class INSIGHT_API IUnknown
		{
		public:
			template <typename T>
			constexpr inline T* GetDerivedType()
			{
				T* pDerived = NULL;

				if (typeid(*this) == typeid(T))
				{
					pDerived = static_cast<T*>(this);
					return pDerived;
				}
				return NULL;
			}

			virtual void Release() {}
		};
	}
}
