#pragma once
#include <Insight/Core.h>
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
		Timer() : m_start_point(clock::now()) {}

		void Reset()
		{
			m_start_point = clock::now();
		}

		clock::rep elapsed_time() const
		{
			clock_duration duration = std::chrono::duration_cast<clock_duration>(clock::now() - m_start_point);
			return duration.count();
		}

		double seconds()
		{
			clock_duration duration = std::chrono::duration_cast<clock_duration>(clock::now() - m_start_point);
			return duration.count() / static_cast<double>(TPeriod::den);
		}

	protected:
		clock::time_point m_start_point;
	};


	class INSIGHT_API FrameTimer : public Timer<>
	{
	public:
		FrameTimer() :
			m_frame_count(0),
			m_fps(0.0f),
			m_prev_frame_time(clock::now()),
			m_start_frame_time(clock::now())
		{ 
		}

		void Tick()
		{
			clock_duration duration = std::chrono::duration_cast<clock_duration>(clock::now() - m_prev_frame_time);
			m_prev_frame_time = clock::now();

			m_dt = duration.count() / static_cast<float>(clock_duration::duration::period::den);

			m_frame_count++;
			if (m_frame_count == 100)
			{
				clock_duration duration = std::chrono::duration_cast<clock_duration>(clock::now() - m_start_frame_time);
				m_start_frame_time = clock::now();

				float elapsed = duration.count() / static_cast<float>(clock_duration::duration::period::den);

				m_fps = static_cast<float>(m_frame_count / elapsed);
				m_frame_count = 0;
			}
		}

		inline float DeltaTime() const
		{
			return m_dt;
		}

		inline float MilliSeconds() const
		{
			return m_dt * 1000.0f;
		}

		inline float FPS() const
		{
			return m_fps;
		}

	private:
		float m_dt = 0.0f;
		float m_fps = 0.0f;

		clock::time_point m_prev_frame_time;
		clock::time_point m_start_frame_time;
		size_t m_frame_count;
	};

	using ns_timer = Timer<std::nano>;
	using mi_timer = Timer<std::micro>;
	using ms_timer = Timer<std::milli>;

}

