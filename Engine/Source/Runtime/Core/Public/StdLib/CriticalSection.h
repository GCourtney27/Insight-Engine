#pragma once

#include "EngineDefines.h"
#include "InsightStd.h"

#include "Platform/Public/PlatformCriticalSection.h"

namespace Insight
{
	

	class INSIGHT_API CriticalSection
	{
	public:
		CriticalSection() = default;
		~CriticalSection() = default;

		FORCEINLINE void Enter();
		FORCEINLINE bool TryEnter();
		FORCEINLINE void Exit();

	private:
		Platform::PlatformCriticalSection m_Mutex;
	};


	//
	// Inline Function Definitions
	//

	FORCEINLINE void CriticalSection::Enter()
	{
		m_Mutex.Lock();
	}

	FORCEINLINE bool CriticalSection::TryEnter()
	{
		return m_Mutex.TryLock();
	}

	FORCEINLINE void CriticalSection::Exit()
	{
		m_Mutex.Unlock();
	}
}
