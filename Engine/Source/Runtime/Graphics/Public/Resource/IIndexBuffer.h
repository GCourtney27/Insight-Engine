#pragma once

#include <Runtime/Core.h>


namespace Insight
{
	namespace Graphics
	{
		class INSIGHT_API IIndexBuffer
		{
		public:
			virtual ~IIndexBuffer() {}

			virtual void* GetNativeBufferView() = 0;

		protected:
			IIndexBuffer() {}
		};
	}
}
