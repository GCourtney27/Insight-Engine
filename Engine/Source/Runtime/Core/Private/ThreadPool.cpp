#include <Engine_pch.h>

#include "Core/Public/ThreadPool.h"

namespace Insight
{

	ThreadPool::ThreadPool(UInt32 NumWorkers, const UInt32* CoreAffinities, UInt32 MaxQueueSize, const UInt64 MaxStackSize)
		: m_NumWorkerThreads(NumWorkers)
		, m_Workers(NULL)
		, m_StackSize(MaxStackSize)
	{
		ZeroMemRanged(m_CoreAffinities, sizeof(m_CoreAffinities));

		if (CoreAffinities != NULL)
			CopyMemRanged(CoreAffinities, &m_CoreAffinities, sizeof(UInt32) * NumWorkers);
		else
			m_CoreAffinities[0] = kInvalidAffinity;

		m_JobQueue.Initialize(MaxQueueSize);
	}

	ThreadPool::~ThreadPool()
	{

		m_JobQueue.UnInitialize();
	}

	void ThreadPool::SpawnWorkers()
	{
		m_Workers = new Worker[m_NumWorkerThreads];
		for (UInt32 i = 0; i < m_NumWorkerThreads; ++i)
		{
			m_Workers[i].Thread = System::CreateAndRunThread(
				"WorkerThread",
				m_CoreAffinities[0] == kInvalidAffinity ? 0 : m_CoreAffinities[i],
				WorkerThreadEntryPoint,
				this,
				m_StackSize);
		}
	}

	void ThreadPool::WorkerThreadEntryPoint(void* pUserData)
	{
		ThreadPool* pThreadPool = RCast<ThreadPool*>(pUserData);
		pThreadPool->WorkerThread();
	}

	void ThreadPool::WorkerThread()
	{
		while (true)
		{
			// Guard resources.
			m_JobQueue.Counter.Aquire();
			m_JobQueue.Guard.Enter();

			System::JobEntryPoint FnPtr;
			void* pUserData = NULL;
			uintptr_t Flags;

			// Initialize the next job from the queue.
			Job& KickedJob = m_JobQueue[m_JobQueue.GetTakeIndex()];
			FnPtr = KickedJob.JobMain;
			pUserData = KickedJob.pUserData;
			Flags = KickedJob.Flags;

			KickedJob.Flags = 0;

			m_JobQueue.Guard.Exit();
			
			if (Flags & JobQuitRequest)
			{
				break;
			}
			else if (Flags & JobRef)
			{

			}
			else
			{
				IE_ASSERT(Flags & JobFuntionPointer); // Invalid flag given when kicking job.
				FnPtr(pUserData);
			}
		}
	}
}
