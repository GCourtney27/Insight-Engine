#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Insight {

	class INSIGHT_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	};

}

// Core log macros
#define IE_CORE_TRACE(...) ::Insight::Log::GetCoreLogger()->trace(__VA_ARGS__);
#define IE_CORE_INFO(...)  ::Insight::Log::GetCoreLogger()->info(__VA_ARGS__);
#define IE_CORE_WARN(...)  ::Insight::Log::GetCoreLogger()->warn(__VA_ARGS__);
#define IE_CORE_ERROR(...) ::Insight::Log::GetCoreLogger()->error(__VA_ARGS__);
#define IE_CORE_FATAL(...) ::Insight::Log::GetCoreLogger()->fatal(__VA_ARGS__);

// Client log macros
#define IE_INFO(...)	   ::Insight::Log::GetClientLogger()->info(__VA_ARGS__);
#define IE_TRACE(...)	   ::Insight::Log::GetClientLogger()->trace(__VA_ARGS__);
#define IE_WARN(...)	   ::Insight::Log::GetClientLogger()->warn(__VA_ARGS__);
#define IE_ERROR(...)	   ::Insight::Log::GetClientLogger()->error(__VA_ARGS__);
#define IE_FATAL(...)	   ::Insight::Log::GetClientLogger()->fatal(__VA_ARGS__);

