#include <Engine_pch.h>

#include "Platform/Public/PlatformSemaphore.h"

namespace Insight
{
	namespace Platform
	{
		PlatformSamaphore::PlatformSamaphore(const UInt32 InitialCount, const UInt32 MaxWaiters, const UInt32 MaxCount)
		{
#if IE_WINDOWS
			m_Semaphore = CreateSemaphore(NULL, InitialCount, MaxCount, NULL);
			IE_ASSERT(m_Semaphore == INVALID_HANDLE_VALUE);
#endif
		}

		PlatformSamaphore::~PlatformSamaphore()
		{
#if IE_WINDOWS
			BOOL Result = CloseHandle(m_Semaphore);
			IE_ASSERT(Result != FALSE);
#endif
		}
		
		void PlatformSamaphore::Aquire()
		{
#if IE_WINDOWS
			DWORD Result = WaitForSingleObject(m_Semaphore, INFINITE);
			IE_ASSERT(Result == WAIT_OBJECT_0);
#endif
		}
		
		bool PlatformSamaphore::TryAquire()
		{
#if IE_WINDOWS
			DWORD Result = WaitForSingleObject(m_Semaphore, 0);
			return Result == WAIT_OBJECT_0;
#endif
			return false;
		}
		
		void PlatformSamaphore::Release(const UInt32 Count/*= 1*/)
		{
#if IE_WINDOWS
			BOOL Result = ReleaseSemaphore(m_Semaphore, Count, NULL);
			IE_ASSERT(Result == TRUE);
#endif
		}
	}
}
