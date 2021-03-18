#pragma once

#include <Runtime/Core.h>
#include <chrono>


#if defined IE_SCOPE_PROFILING_ENABLED
// Scoped timer whos base output method is milliseconds.
#	define ScopeTimer(Message, OutputType) Insight::Profiling::ScopedTimer Timer(Message, Insight::Profiling::EOutputType::OT_##OutputType);

// Scoped timer whos base output method is seconds.
#	define ScopedSecondTimer(Message) Insight::Profiling::ScopedTimer Timer(Message, Insight::Profiling::EOutputType::OT_Seconds);

// Scoped timer whos base output method is miliseconds.
#	define ScopedMilliSecondTimer(Message) Insight::Profiling::ScopedTimer Timer(Message, Insight::Profiling::EOutputType::OT_Milliseconds);

// Scoped timer whos base output method is nanoseconds.
#	define ScopedNanoSecondTimer(Message) Insight::Profiling::ScopedTimer Timer(Message, Insight::Profiling::EOutputType::OT_NanoSeconds);

// Scoped timer whos base output method is microseconds.
#	define ScopedMicroSecondTimer(Message) Insight::Profiling::ScopedTimer Timer(Message, Insight::Profiling::EOutputType::OT_MicroSeconds);
#else
#	define ScopeTimer(Message, EOutputType)
#	define ScopedMilliSecondTimer(Message)
#	define ScopedSecondTimer(Message)
#	define ScopedNanoSecondTimer(Message)
#endif

namespace Insight {

	namespace Profiling {

		enum class EOutputType
		{
			OT_Seconds,
			OT_Milliseconds,
			OT_NanoSeconds,
			OT_MicroSeconds
		};

		/*
			Outlines a timer with start and stop capabilities.
		*/
		template <typename TimeType>
		struct INSIGHT_API Timer
		{
			Timer(EOutputType OutputType)
				: m_OutputType(OutputType)
				, m_Started(false)
			{
			}
			virtual ~Timer() = default;
			
			/*
				Starts the timer.
			*/
			FORCE_INLINE void Start()
			{
				if (!GetIsTimerStarted())
				{
					SetTimerStarted(true);
					m_Start = std::chrono::steady_clock::now();
				}
				else
				{
					IE_LOG(Warning, TEXT("Timer already started!"));
				}
			}

			/*
				Stops the timer and calculates the amount of time elapsed since Timer::Start().
			*/
			FORCE_INLINE void Stop()
			{
				if (GetIsTimerStarted())
				{
					SetTimerStarted(false);
					m_End = std::chrono::steady_clock::now();
					m_Duration = m_End - m_Start;
				}
				else
				{
					IE_LOG(Warning, TEXT("Timer being stopped without being started!"))
				}
			}

			/*
				Returns true if the timer has started or false if not.
			*/
			FORCE_INLINE bool GetIsTimerStarted() const 
			{
				return m_Started;
			}

			/*
				Set if the timer has started.
				@param Started - Weather the timer has been started.
			*/
			FORCE_INLINE void SetTimerStarted(bool Started)
			{
				m_Started = Started;
			}

			/*
				Returns the amount of time the timer tracked in nanoseconds.
			*/
			FORCE_INLINE TimeType GetElapsedNanos() const
			{
				return (TimeType)std::chrono::duration_cast<std::chrono::nanoseconds>(m_Duration).count();
			}

			/*
				Returns the amount of time the timer tracked in milliseconds.
			*/
			FORCE_INLINE TimeType GetElapsedMiliSeconds() const
			{
				return (TimeType)std::chrono::duration_cast<std::chrono::milliseconds>(m_Duration).count();
			}

			/*
				Returns the amount of time the timer tracked in seconds.
			*/
			FORCE_INLINE TimeType GetElepsedSeconds() const
			{
				return (TimeType)std::chrono::duration_cast<std::chrono::seconds>(m_Duration).count() + (GetElapsedMiliSeconds() / 1000.0);
			}

			/*
				Returns the amount of time the timer tracked in microseconds.
			*/
			FORCE_INLINE TimeType GetElepsedMicroSeconds() const
			{
				return (TimeType)std::chrono::duration_cast<std::chrono::microseconds>(m_Duration).count();
			}


		protected:
			bool m_Started;
			EOutputType m_OutputType;
			std::chrono::duration<TimeType> m_Duration;
			std::chrono::time_point<std::chrono::steady_clock> m_Start, m_End;
		};

		struct INSIGHT_API ScopedTimer : private Timer<double>
		{
			ScopedTimer(const TChar* ScopeName, EOutputType OutputType = EOutputType::OT_Milliseconds)
				: m_ScopeName(ScopeName), Timer(OutputType)
			{
				Start();
			}
			virtual ~ScopedTimer()
			{
				Stop();
				switch (m_OutputType)
				{
				case EOutputType::OT_Milliseconds:
					IE_LOG(Log, TEXT("%s took %f milliseconds"), m_ScopeName, GetElapsedMiliSeconds());
					break;
				case EOutputType::OT_Seconds:
					IE_LOG(Log, TEXT("%s took %f seconds"), m_ScopeName, GetElepsedSeconds());
					break;
				case EOutputType::OT_NanoSeconds:
					IE_LOG(Log, TEXT("%s took %f nanoseconds"), m_ScopeName, GetElapsedNanos());
					break;
				case EOutputType::OT_MicroSeconds:
					IE_LOG(Log, TEXT("%s took %f microseconds"), m_ScopeName, GetElepsedMicroSeconds());
					break;
				default:
					break;
				}
			}

		private:
			const TChar* m_ScopeName;
		};

	} // End namespace Profiling

}// End namspace Insight
