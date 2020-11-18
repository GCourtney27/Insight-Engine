#pragma once

#include <Insight/Core.h>
#ifdef IE_PLATFORM_BUILD_WIN32
#include "Platform/Win32/Console_Window.h"
#endif 

#pragma warning (disable : 26451)
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Insight {

	namespace Debug {


		class INSIGHT_API Logger
		{
		public:
			static bool Init();
			~Logger() { }

			inline static void HoldForUserInput() { system("PAUSE"); }

			inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
			inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		private:
			static std::shared_ptr<spdlog::logger> s_CoreLogger;
			static std::shared_ptr<spdlog::logger> s_ClientLogger;
#if defined (IE_DEBUG) && defined (IE_PLATFORM_BUILD_WIN32)
			static ConsoleWindow m_ConsoleWindow;
#endif
		};

		
	}

}

enum class LogSeverity
{
	Log,
	Verbose,
	Warning,
	Error,
	Critical,
};

namespace Debug {

	// Logger that will log output to the console window. Recommended that you dont call directly. 
	// Instead, use IE_DEBUG_LOG so logs will be stripped from release builds.
	template <typename ... Params>
	inline constexpr void Log(LogSeverity Severity, Params ... params)
	{
		switch (Severity)
		{
		case LogSeverity::Log:
			Insight::Debug::Logger::GetCoreLogger()->info(params...);
			break;
		case LogSeverity::Verbose:
			Insight::Debug::Logger::GetCoreLogger()->trace(params...);
			break;
		case LogSeverity::Warning:
			Insight::Debug::Logger::GetCoreLogger()->warn(params...);
			break;
		case LogSeverity::Error:
			Insight::Debug::Logger::GetCoreLogger()->error(params...);
			break;
		case LogSeverity::Critical:
			Insight::Debug::Logger::GetCoreLogger()->critical(params...);
			break;
		default:
			Insight::Debug::Logger::GetCoreLogger()->warn("Invalid log severity given to logger. Choose one option from Log::LogSeverity enum when Debug Logging.");
			break;
		}
	}
}


#if defined IE_DEBUG || defined IE_RELEASE
#define IE_FATAL_ERROR(...) __debugbreak(); OutputDebugString(__VA_ARGS__)
#define IE_DEBUG_LOG(Severity, ...) ::Debug::Log(Severity, __VA_ARGS__);
#else
#define IE_FATAL_ERROR(...)
#define IE_DEBUG_LOG(LogSeverity, ...)
#endif
