#include <Engine_pch.h>

#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Insight {

	namespace Debug {

		std::shared_ptr<spdlog::logger> Logger::s_CoreLogger;
		std::shared_ptr<spdlog::logger> Logger::s_WarningLogger;
		std::shared_ptr<spdlog::logger> Logger::s_ErrorLogger;
		std::shared_ptr<spdlog::logger> Logger::s_ClientLogger;
#if defined IE_DEBUG && defined IE_PLATFORM_BUILD_WIN32
		ConsoleWindow Logger::m_ConsoleWindow;
#endif

		bool Logger::Init()
		{
			spdlog::set_pattern("%^[%T] [Insight] [%n] %v%$");
			s_CoreLogger = spdlog::stdout_color_mt("Log");
			s_CoreLogger->set_level(spdlog::level::trace);

			s_WarningLogger = spdlog::stdout_color_mt("Warning");
			s_WarningLogger->set_level(spdlog::level::trace);

			s_ErrorLogger = spdlog::stdout_color_mt("Error");
			s_ErrorLogger->set_level(spdlog::level::trace);

			s_ClientLogger = spdlog::stdout_color_mt("App");
			s_ClientLogger->set_level(spdlog::level::trace);

			return true;
		}

	}
}

