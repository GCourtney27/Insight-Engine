#include <Engine_pch.h>

#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Insight {

	namespace Debug {

		std::shared_ptr<spdlog::logger> Logger::s_CoreLogger;
		std::shared_ptr<spdlog::logger> Logger::s_ClientLogger;
#if defined IE_DEBUG || defined IE_RELEASE
		ConsoleWindow Logger::m_ConsoleWindow;
#endif

		bool Logger::Init()
		{
			spdlog::set_pattern("%^[%T] %n: %v%$");
			s_CoreLogger = spdlog::stdout_color_mt("Insight");
			s_CoreLogger->set_level(spdlog::level::trace);

			s_ClientLogger = spdlog::stdout_color_mt("App");
			s_ClientLogger->set_level(spdlog::level::trace);

			return true;
		}

	}
}

