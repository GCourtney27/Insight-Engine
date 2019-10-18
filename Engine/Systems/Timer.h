#pragma once
#include <chrono>
#include "..\\Framework\\Singleton.h"
#include "..\\Engine.h"

class Timer : public Singleton<Timer>
{
public:
	Timer();
	bool Initialize(Engine* engine);


	double GetTicks();
	void Restart();

	bool Start();
	bool Stop();
private:
	Engine* m_engine;

	bool isrunning = false;
	float m_dt;
	float m_timeScale;

#ifdef _WIN32
	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> stop;
#else
	std::chrono::time_point<std::chrono::system_clock> start;
	std::chrono::time_point<std::chrono::system_clock> stop;
#endif
};