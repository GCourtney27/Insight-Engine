#pragma once

#include "EngineDefines.h"
#include "InsightStd.h"

#include "Platform/Public/PlatformMutex.h"


namespace Insight
{
	class INSIGHT_API Mutex
	{
	public:
		Mutex() = default;
		~Mutex() = default;

		FORCEINLINE void Lock();
		FORCEINLINE void Unlock();
		FORCEINLINE bool TryLock();

	protected:
		Platform::PlatformMutex m_Mutex;
	};


	//
	// Inline Funtion Implementation
	//

	FORCEINLINE void Mutex::Lock()
	{
		m_Mutex.Lock();
	}

	FORCEINLINE void Mutex::Unlock()
	{
		m_Mutex.Unlock();
	}

	FORCEINLINE bool Mutex::TryLock()
	{
		return m_Mutex.TryLock();
	}
}
