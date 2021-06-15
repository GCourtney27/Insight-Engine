#pragma once

#include "EngineDefines.h"

namespace Insight
{
	namespace Platform
	{
		class INSIGHT_API PlatformMutex
		{
		public:
			PlatformMutex();
			~PlatformMutex();

			void Lock();
			void Unlock();
			bool TryLock();

		protected:
#if IE_WINDOWS
			HANDLE m_Mutex;
#endif
		};
	}
}
