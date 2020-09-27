#pragma once

#include <Retina/Core.h>
#ifdef RN_PLATFORM_WINDOWS
	#include "Platform/Windows/Console_Window.h"
#endif 

#pragma warning (disable : 26451)
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Retina {

	class RETINA_API Log
	{
	public:
		static bool Init();
		~Log() { }

		inline static void HoldForUserInput() { system("PAUSE"); }

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
#if defined RN_DEBUG || defined RN_RELEASE
		static ConsoleWindow m_ConsoleWindow;
#endif
	};

}

#if defined RN_DEBUG || defined RN_RELEASE
	// Engine log macros
	#define RN_CORE_TRACE(...) ::Retina::Log::GetCoreLogger()->trace(__VA_ARGS__);
	#define RN_CORE_INFO(...)  ::Retina::Log::GetCoreLogger()->info(__VA_ARGS__);
	#define RN_CORE_WARN(...)  ::Retina::Log::GetCoreLogger()->warn(__VA_ARGS__);
	#define RN_CORE_ERROR(...) ::Retina::Log::GetCoreLogger()->error(__VA_ARGS__);
	#define RN_CORE_FATAL(...) __debugbreak(); OutputDebugString(__VA_ARGS__)

	// Client log macros
	#define RN_INFO(...)	   ::Retina::Log::GetClientLogger()->info(__VA_ARGS__);
	#define RN_TRACE(...)	   ::Retina::Log::GetClientLogger()->trace(__VA_ARGS__);
	#define RN_WARN(...)	   ::Retina::Log::GetClientLogger()->warn(__VA_ARGS__);
	#define RN_ERROR(...)	   ::Retina::Log::GetClientLogger()->error(__VA_ARGS__);
	#define RN_FATAL(...)	   __debugbreak(); OutputDebugString(__VA_ARGS__)
#else
	// Engine logging
	#define RN_CORE_TRACE
	#define RN_CORE_INFO
	#define RN_CORE_WARN
	#define RN_CORE_ERROR
	#define RN_CORE_FATAL

	// Client logging
	#define RN_INFO
	#define RN_TRACE
	#define RN_WARN
	#define RN_ERROR
	#define RN_FATAL
#endif
