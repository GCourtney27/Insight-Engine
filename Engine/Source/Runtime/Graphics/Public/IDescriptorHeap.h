#pragma once

#include <Runtime/Core.h>
#include <Runtime/Graphics/Public/GraphicsCore.h>


namespace Insight
{
	namespace Graphics
	{
		class INSIGHT_API IDescriptorHeap
		{
		public:
			virtual void* GetNativeHeap() = 0;

		protected:
			IDescriptorHeap() {}
			virtual ~IDescriptorHeap() {}

			virtual void Create(const EString& DebugHeapName, EResourceHeapType Type, uint32_t MaxCount) = 0;

		};
	}
}
