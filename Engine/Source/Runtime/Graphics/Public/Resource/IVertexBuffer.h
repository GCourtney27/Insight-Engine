#pragma once

#include <Runtime/Core.h>

namespace Insight
{
	namespace Graphics
	{
		class INSIGHT_API IVertexBuffer
		{
		public:
			virtual void* GetNativeBufferView() = 0;


		protected:
			IVertexBuffer() {}
			~IVertexBuffer() {}

		};
	}
}
