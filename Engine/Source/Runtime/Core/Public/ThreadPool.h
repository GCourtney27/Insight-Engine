#pragma once

#include "EngineDefines.h"
#include "Platform/Public/System.h"

#include "Core/Public/StdLib/CriticalSection.h"
#include "Core/Public/StdLib/Semaphore.h"

namespace Insight
{
	class INSIGHT_API ThreadPool
	{
	public:
		ThreadPool(UInt32 NumWorkers, const UInt32* CoreAffinities, UInt32 MaxQueueSize = 16, const UInt64 MaxStackSize = System::kDefaultStackSize);
		~ThreadPool();

		void SpawnWorkers();

	protected:
		static void WorkerThreadEntryPoint(void* pUserData);
		void WorkerThread();

		struct Worker
		{
			System::ThreadId Thread;
		};
		struct Job
		{
			Job()
				: Flags(0u)
				, pUserData(NULL)
				, JobMain(NULL)
			{
			}
			UInt64					Flags;
			void*					pUserData;
			System::JobEntryPoint	JobMain;
		};
		struct JobQueue
		{
			const UInt32 kInvalidJobQueueSize = (UInt32)-1;
			JobQueue()
				: m_Queue(NULL)
				, m_AddIndex(0)
				, m_TakeIndex(0)
				, m_MaxQueueSize(kInvalidJobQueueSize)
			{
			}
			~JobQueue()
			{
				if(IsValid())
					UnInitialize();
			}

			inline void Initialize(UInt32 MaxQueueSize);
			inline void UnInitialize();

			inline bool IsValid();
			inline UInt32 GetTakeIndex();
			inline UInt32 GetAddIndex() const;
			inline Job& operator[](UInt32 Index);

			CriticalSection Guard;
			Semaphore		Counter;

		private:
			Job*			m_Queue;
			UInt32			m_AddIndex;
			UInt32			m_TakeIndex;
			UInt32			m_MaxQueueSize;
		};
		enum
		{
			kMaxParallelThreads = 12,
			kInvalidAffinity	= 0xFFFFFFFF,
		};
		enum JobUserDataValue
		{
			JobFuntionPointer,
			JobRef,
			JobQuitRequest,
		};


		UInt32	m_NumWorkerThreads;
		Worker* m_Workers;
		UInt64	m_StackSize;
		UInt32	m_CoreAffinities[kMaxParallelThreads];

		CriticalSection m_JobQueueGuard;
		Semaphore		m_JobQueueCounter;
		JobQueue		m_JobQueue;
	};


	//
	// Inline Function Implementations
	//


	inline void ThreadPool::JobQueue::Initialize(UInt32 MaxQueueSize)
	{
		UnInitialize();

		this->m_MaxQueueSize = MaxQueueSize;
		m_Queue = new Job[MaxQueueSize];
	}

	inline void ThreadPool::JobQueue::UnInitialize()
	{
		if (IsValid())
		{
			SAFE_DELETE_PTR_ARRAY(m_Queue);
			m_MaxQueueSize = kInvalidJobQueueSize;
		}
	}

	inline bool ThreadPool::JobQueue::IsValid()
	{
		return ( m_Queue != NULL ) && (m_MaxQueueSize != kInvalidJobQueueSize);
	}

	inline UInt32 ThreadPool::JobQueue::GetTakeIndex()
	{
		UInt32 Return = m_TakeIndex;
		
		if (++m_TakeIndex == m_MaxQueueSize)
		{
			m_TakeIndex = 0;
		}
		return Return;
	}

	inline UInt32 ThreadPool::JobQueue::GetAddIndex() const
	{
		return m_AddIndex;
	}

	inline ThreadPool::Job& ThreadPool::JobQueue::operator[](UInt32 Index)
	{
		return m_Queue[GetTakeIndex()];
	}
}