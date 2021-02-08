#pragma once

#include <Runtime/CoreMacros.h>
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
			inline static std::shared_ptr<spdlog::logger>& GetWarningLogger() { return s_WarningLogger; }
			inline static std::shared_ptr<spdlog::logger>& GetErrorLogger() { return s_ErrorLogger; }
			inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		private:
			static std::shared_ptr<spdlog::logger> s_CoreLogger;
			static std::shared_ptr<spdlog::logger> s_WarningLogger;
			static std::shared_ptr<spdlog::logger> s_ErrorLogger;
			static std::shared_ptr<spdlog::logger> s_ClientLogger;
#if defined (IE_DEBUG) && defined (IE_PLATFORM_BUILD_WIN32)
			static ConsoleWindow m_ConsoleWindow;
#endif
		};

		
	}

}

enum class ELogSeverity
{
	Log,
	Verbose,
	Warning,
	Error,
	Critical,
};

namespace Debug {

	// Logger that will log output to the console window. Recommended that you dont call directly. 
	// Instead, use IE_LOG so logs will be stripped from release builds.
	template <typename ... LogContents>
	inline constexpr void LogHelper(ELogSeverity Severity = ELogSeverity::Log, LogContents ... Contents)
	{
		switch (Severity)
		{
		case ELogSeverity::Log:
			Insight::Debug::Logger::GetCoreLogger()->info(Contents...);
			break;
		case ELogSeverity::Verbose:
			Insight::Debug::Logger::GetCoreLogger()->trace(Contents...);
			break;
		case ELogSeverity::Warning:
			Insight::Debug::Logger::GetWarningLogger()->warn(Contents...);
			break;
		case ELogSeverity::Error:
			Insight::Debug::Logger::GetErrorLogger()->error(Contents...);
			break;
		case ELogSeverity::Critical:
			Insight::Debug::Logger::GetCoreLogger()->critical(Contents...);
			break;
		default:
			Insight::Debug::Logger::GetCoreLogger()->warn("Invalid log severity given to logger. Choose one option from Log::LogSeverity enum.");
			break;
		}
	}
}


#if defined (IE_DEBUG) || defined (IE_RELEASE)
#	define IE_FATAL_ERROR(...) __debugbreak(); OutputDebugString(__VA_ARGS__)
#	if IE_PLATFORM_BUILD_WIN32
#		define IE_LOG(Severity, ...) ::Debug::LogHelper(ELogSeverity::##Severity, __VA_ARGS__);
#	elif IE_PLATFORM_BUILD_UWP
#		define WIDE_STRING(...) L#__VA_ARGS__
#		define IE_LOG(Severity, ...) {wchar_t Buffer[512]; swprintf(Buffer, sizeof(Buffer), WIDE_STRING(__VA_ARGS__)); OutputDebugString(Buffer);}
#	endif
#else
#	define IE_FATAL_ERROR(...)
#	define IE_LOG(LogSeverity, ...)
#endif
