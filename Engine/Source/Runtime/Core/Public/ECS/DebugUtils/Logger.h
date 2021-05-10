#pragma once

#include "ECS/Core/Core.h"

#include <stdio.h>

namespace ECS
{
	namespace Debug
	{
		enum class LogSeverity
		{
			Log,		// Simple debug log message.
			Verbose,	// More advanced message containing line and file information on where the log happened.
			Warning,	// Verbose message, warning the user should consider fixing what caused the log as soon as possible
			Fatal,		// Crashes the program and displays the error message.
		};

		struct Logger
		{

		};

		inline void LogMessage(LogSeverity Severity, const char* fmt)
		{
			switch (Severity)
			{
			case Debug::LogSeverity::Log:
				printf("[ECS Core][Log] - %s", fmt);
					break;
			case Debug::LogSeverity::Verbose:
				break;
			case Debug::LogSeverity::Warning:
				break;
			case Debug::LogSeverity::Fatal:
				break;
			default:
				break;
			}
		}
	}
}
#define ECS_LOG(Message, ...) 
#if defined (DEBUG)
#endif // DEBUG
