#include "Timer.h"
#include <algorithm>

Timer::Timer()
{
}

bool Timer::Initialize(Engine * engine)
{
	start = std::chrono::high_resolution_clock::now();
	stop = std::chrono::high_resolution_clock::now();
	m_timeScale = 1.0f;
	m_isrunning = true;

	return true;
}

void Timer::Update()
{
	UINT32 ticks = GetTicks();
	UINT32 milliseconds = ticks - m_prevticks;
	m_prevticks = ticks;

	m_dt = (m_isrunning) ? 0.0f : milliseconds / 1000.0f;
	m_dt = std::min<float>(m_dt, 1.0f);

}

UINT32 Timer::GetAppTime()
{
	auto start_time = std::chrono::high_resolution_clock::now();
	auto current_time = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
	return time;
}

double Timer::GetTicks()
{
	if (m_isrunning)
	{
		auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start);
		return elapsed.count();
	}
	else
	{
		auto elapsed = std::chrono::duration<double, std::milli>(stop - start);
		return elapsed.count() / m_timeScale;
	}
}

void Timer::Restart()
{
	m_isrunning = true;
	start = std::chrono::high_resolution_clock::now();
}

bool Timer::Stop()
{
	if (!m_isrunning)
	{
		return false;
	}
	else
	{
		stop = std::chrono::high_resolution_clock::now();
		m_isrunning = false;
		return true;
	}
	return true;
}

bool Timer::Start()
{
	if (m_isrunning)
	{
		return false;
	}
	else
	{
		start = std::chrono::high_resolution_clock::now();
		m_isrunning = true;
		return true;
	}
}
