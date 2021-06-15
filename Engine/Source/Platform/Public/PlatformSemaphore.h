#pragma once

#include "EngineDefines.h"

#define IE_DEFAULT_SEMAPHORE_MAX_COUNT 100000

namespace Insight
{
	namespace Platform
	{
		class INSIGHT_API PlatformSamaphore
		{
		public:
			PlatformSamaphore(const UInt32 InitialCount = 0, const UInt32 MaxWaiters = 16, const UInt32 MaxCount = IE_DEFAULT_SEMAPHORE_MAX_COUNT);
			~PlatformSamaphore();

			void Aquire();
			bool TryAquire();
			void Release(const UInt32 Count = 1);

			FORCEINLINE void Reset();
			
		protected:
#if IE_WINDOWS
			HANDLE m_Semaphore;
#endif
		};


		//
		// Inline Funciton Implementations
		//

		FORCEINLINE void PlatformSamaphore::Reset()
		{
			do
			{
			} while (TryAquire());
		}
	}
}
