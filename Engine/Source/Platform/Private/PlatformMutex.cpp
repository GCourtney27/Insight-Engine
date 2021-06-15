#include <Engine_pch.h>

#include "Platform/Public/PlatformMutex.h"

namespace Insight
{
	namespace Platform
	{
		PlatformMutex::PlatformMutex()
		{
#if IE_WINDOWS
			m_Mutex = CreateMutex(NULL, FALSE, NULL);
			IE_ASSERT(m_Mutex != NULL);
#endif
		}

		PlatformMutex::~PlatformMutex()
		{
#if IE_WINDOWS
			BOOL Result = CloseHandle(m_Mutex);
			IE_ASSERT(Result == TRUE);
#endif
		}
		
		void PlatformMutex::Lock()
		{
#if IE_WINDOWS
			UInt32 Result = WaitForSingleObject(m_Mutex, INFINITE);
			IE_ASSERT(Result == WAIT_OBJECT_0);
#endif
		}
		
		void PlatformMutex::Unlock()
		{
#if IE_WINDOWS
			BOOL Result = ReleaseMutex(m_Mutex);
			IE_ASSERT(Result == TRUE);
#endif
		}
		
		bool PlatformMutex::TryLock()
		{
#if IE_WINDOWS
			return WaitForSingleObject(m_Mutex, 0) == WAIT_OBJECT_0;
#endif
			return false;
		}
	}
}
