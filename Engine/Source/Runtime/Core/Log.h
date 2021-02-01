#pragma once

#include <Runtime/Core.h>
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
	template <typename ... LogContents>
	inline constexpr void Log(LogSeverity Severity = LogSeverity::Log, LogContents ... Contents)
	{
		switch (Severity)
		{
		case LogSeverity::Log:
			Insight::Debug::Logger::GetCoreLogger()->info(Contents...);
			break;
		case LogSeverity::Verbose:
			Insight::Debug::Logger::GetCoreLogger()->trace(Contents...);
			break;
		case LogSeverity::Warning:
			Insight::Debug::Logger::GetCoreLogger()->warn(Contents...);
			break;
		case LogSeverity::Error:
			Insight::Debug::Logger::GetCoreLogger()->error(Contents...);
			break;
		case LogSeverity::Critical:
			Insight::Debug::Logger::GetCoreLogger()->critical(Contents...);
			break;
		default:
			Insight::Debug::Logger::GetCoreLogger()->warn("Invalid log severity given to logger. Choose one option from Log::LogSeverity enum.");
			break;
		}
	}
}


#if defined (IE_DEBUG) || defined (IE_RELEASE)
	#define IE_FATAL_ERROR(...) __debugbreak(); OutputDebugString(__VA_ARGS__)
	#if IE_PLATFORM_BUILD_WIN32
		#define IE_DEBUG_LOG(Severity, ...) ::Debug::Log(Severity, __VA_ARGS__);
	#elif IE_PLATFORM_BUILD_UWP
		#define WIDE_STRING(...) L#__VA_ARGS__
		#define IE_DEBUG_LOG(Severity, ...) {wchar_t Buffer[512]; swprintf(Buffer, sizeof(Buffer), WIDE_STRING(__VA_ARGS__)); OutputDebugString(Buffer);}
	#endif
#else
	#define IE_FATAL_ERROR(...)
	#define IE_DEBUG_LOG(LogSeverity, ...)
#endif
