#pragma once

#include "EngineDefines.h"
#include "InsightStd.h"

#include "Platform/Public/PlatformSemaphore.h"


namespace Insight
{
	class INSIGHT_API Semaphore
	{
	public:
		Semaphore() = default;
		~Semaphore() = default;

		FORCEINLINE void Aquire();
		FORCEINLINE bool TryAquire();
		FORCEINLINE void Release(const UInt32 Count = 1);

		FORCEINLINE void Reset();

	protected:
		Platform::PlatformSamaphore m_Semaphore;
	};


	//
	// Inline Function Implementations
	//

	FORCEINLINE void Semaphore::Aquire()
	{
		m_Semaphore.Aquire();
	}

	FORCEINLINE bool Semaphore::TryAquire()
	{
		m_Semaphore.TryAquire();
	}

	FORCEINLINE void Semaphore::Release(const UInt32 Count/* = 1*/)
	{
		m_Semaphore.Release(Count);
	}

	FORCEINLINE void Semaphore::Reset()
	{
		m_Semaphore.Reset();
	}
}
