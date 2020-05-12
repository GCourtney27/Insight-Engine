#include <ie_pch.h>

#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Insight {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
#ifdef IE_DEBUG
	ConsoleWindow Log::m_ConsoleWindow;
#endif

	bool Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_CoreLogger = spdlog::stdout_color_mt("Insight");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("App");
		s_ClientLogger->set_level(spdlog::level::trace);

		return true;
	}

}

