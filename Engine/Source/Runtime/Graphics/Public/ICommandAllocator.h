#pragma once

#include "EngineDefines.h"

namespace Insight
{
	namespace Graphics
	{
		class INSIGHT_API ICommandAllocator
		{
		public:
			virtual void* GetNativeAllocator() = 0;

			virtual void Initialize() = 0;

		protected:
			ICommandAllocator() {}
			virtual ~ICommandAllocator() {}
		};
	}
}
