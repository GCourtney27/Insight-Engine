#pragma once
#include <Runtime/Core.h>
#include <chrono>

namespace Insight {

	// TODO: This timer is currently platform independent.
	// It should however be dependent on the OS for higher 
	// more accurate performance.

	template<typename TPeriod = std::chrono::high_resolution_clock::period>
	class INSIGHT_API Timer
	{
	public:
		using clock = std::chrono::high_resolution_clock;
		using clock_duration = std::chrono::duration<clock::rep, TPeriod>;

	public:
		Timer() : m_StartPoint(clock::now()) {}

		void Reset()
		{
			m_StartPoint = clock::now();
		}

		clock::rep ElapsedTime() const
		{
			clock_duration duration = std::chrono::duration_cast<clock_duration>(clock::now() - m_StartPoint);
			return duration.count();
		}

		double Seconds()
		{
			clock_duration duration = std::chrono::duration_cast<clock_duration>(clock::now() - m_StartPoint);
			return duration.count() / static_cast<double>(TPeriod::den);
		}

	protected:
		clock::time_point m_StartPoint;
	};


	class INSIGHT_API FrameTimer : public Timer<>
	{
	public:
		FrameTimer() :
			m_FrameCount(0),
			m_FramesPerSecond(0.0f),
			m_PrevFrameTime(clock::now()),
			m_StartFrameTime(clock::now())
		{ 
		}

		void Tick()
		{
			clock_duration duration = std::chrono::duration_cast<clock_duration>(clock::now() - m_PrevFrameTime);
			m_PrevFrameTime = clock::now();

			m_DeltaTime = duration.count() / static_cast<float>(clock_duration::duration::period::den);

			m_FrameCount++;
			if (m_FrameCount == 100)
			{
				clock_duration duration = std::chrono::duration_cast<clock_duration>(clock::now() - m_StartFrameTime);
				m_StartFrameTime = clock::now();

				float elapsed = duration.count() / static_cast<float>(clock_duration::duration::period::den);

				m_FramesPerSecond = static_cast<float>(m_FrameCount / elapsed);
				m_FrameCount = 0;
			}
		}

		inline float DeltaTime() const
		{
			return m_DeltaTime;
		}

		inline float MilliSeconds() const
		{
			return m_DeltaTime * 1000.0f;
		}

		template <typename ConversionType = float>
		inline ConversionType FPS() const
		{
			return static_cast<ConversionType>(m_FramesPerSecond);
		}

	private:
		float m_DeltaTime = 0.0f;
		float m_FramesPerSecond = 0.0f;

		clock::time_point m_PrevFrameTime;
		clock::time_point m_StartFrameTime;
		size_t m_FrameCount;
	};

	using ns_timer = Timer<std::nano>;
	using mi_timer = Timer<std::micro>;
	using ms_timer = Timer<std::milli>;

}

