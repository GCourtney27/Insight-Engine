#pragma once

#include "EngineDefines.h"

#if !IE_WINDOWS
#	include "Core/Public/StdLib/Mutex.h"
#endif

namespace Insight
{
	namespace Platform
	{
		class INSIGHT_API PlatformCriticalSection
		{
		public:
			PlatformCriticalSection();
			~PlatformCriticalSection();

			/*
				Locks the mutex.
			*/
			void Lock();

			/*
				Returns true if the mutex was locked successfuly. False if not.
			*/
			bool TryLock();

			/*
				Unlocks the mutex.
			*/
			void Unlock();

		private:
#if IE_WINDOWS
			CRITICAL_SECTION m_Mutex;
#else
			Mutex m_Mutex;
#endif
		};
	}
}
