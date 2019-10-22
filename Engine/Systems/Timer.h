#pragma once
#include <chrono>
#include "..\\Framework\\Singleton.h"
#include "..\\Engine.h"

class Timer : public Singleton<Timer>
{
public:
	Timer();
	bool Initialize(Engine* engine);
	void Update();

	double GetTicks();
	void Restart();
	UINT32 GetAppTime();

	float GetDeltaTime() { return m_dt * m_timeScale; }
	float GetDeltaTimeUnsaled() { return m_dt; }

	bool Start();
	bool Stop();
private:
	Engine* m_engine;

	float m_dt;
	float m_timeScale;
	UINT32 m_prevticks;
	bool m_isrunning = true;

#ifdef _WIN32
	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> stop;
#else
	std::chrono::time_point<std::chrono::system_clock> appStart;
	std::chrono::time_point<std::chrono::system_clock> stop;
#endif
};