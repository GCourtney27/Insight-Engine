#include <Engine_pch.h>

#include "Platform/Public/PlatformCriticalSection.h"

namespace Insight
{
	namespace Platform
	{
		PlatformCriticalSection::PlatformCriticalSection()
		{
			BOOL Result = InitializeCriticalSectionAndSpinCount(&m_Mutex, 0x80000400);
#if IE_WIN32
			IE_ASSERT(SUCCEEDED(Result));
#endif
		}
		
		PlatformCriticalSection::~PlatformCriticalSection()
		{
#if IE_WINDOWS
			DeleteCriticalSection(&m_Mutex);
#endif
		}

		void PlatformCriticalSection::Lock()
		{
#if IE_WINDOWS
			EnterCriticalSection(&m_Mutex);
#else
			m_Mutex.Lock();
#endif
		}

		bool PlatformCriticalSection::TryLock()
		{
#if IE_WINDOWS
			return TryEnterCriticalSection(&m_Mutex) == TRUE;
#else
			m_Mutex.TryLock();
#endif
		}

		void PlatformCriticalSection::Unlock()
		{
#if IE_WINDOWS
			LeaveCriticalSection(&m_Mutex);
#else
			m_Mutex.Unlock();
#endif
		}
	}
}
