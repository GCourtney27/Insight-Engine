#include <Engine_pch.h>

#include "Log.h"


namespace Insight {

	namespace Debug {

#if defined IE_DEBUG && defined IE_PLATFORM_BUILD_WIN32
		ConsoleWindow Logger::s_ConsoleWindow;
#endif

		bool Logger::Initialize()
		{
			// Initialize the console window.
			ConsoleWindowDesc WindowDesc;
			WindowDesc.CanClose = false;
			WindowDesc.BufferDims = ieFloat2(700, 320);
			WindowDesc.WindowDims = ieFloat2(170, 42);
			WindowDesc.DefaultForegroundColor = CC_White;
			bool ConsoleCreated = s_ConsoleWindow.Create(WindowDesc);

			return ConsoleCreated;
		}




		void LogHelper(ELogSeverity Severity, const char* fmt, const char* File, const char* Function, int Line, ...)
		{
			char TraceBuffer[1024];
			char OutputBuffer[1024];

			// Initialize the output message buffer.
			va_list args;
			va_start(args, Line); // Start capturing arguments after the 'Line' parameter in the method.
			{
				_vsnprintf(OutputBuffer, sizeof(OutputBuffer), fmt, args);
			}
			va_end(args);

			switch (Severity)
			{
			case ELogSeverity::Log:
				sprintf_s(TraceBuffer, "[Insight][Log] - ");
				break;
			case ELogSeverity::Verbose:
				Insight::Debug::Logger::GetConsoleWindow().SetForegroundColor(Insight::EConsoleColor::CC_Green);
				sprintf_s(TraceBuffer, "[Insight][Verbose][%s-%s-%i] - ", File, Function, Line);
				break;
			case ELogSeverity::Warning:
				Insight::Debug::Logger::GetConsoleWindow().SetForegroundColor(Insight::EConsoleColor::CC_Yellow);
				sprintf_s(TraceBuffer, "[Insight][Warning] - ");
				break;
			case ELogSeverity::Error:
				Insight::Debug::Logger::GetConsoleWindow().SetForegroundColor(Insight::EConsoleColor::CC_Red);
				sprintf_s(TraceBuffer, "[Insight][Error] - ");
				break;
			case ELogSeverity::Critical:
				Insight::Debug::Logger::GetConsoleWindow().SetForegroundColor(Insight::EConsoleColor::CC_Orange);
				sprintf_s(TraceBuffer, "[Insight][Critical] - ");
				break;
			default:
				sprintf_s(TraceBuffer, "Invalid log severity given to logger. Choose one option from Log::LogSeverity enum.");
				break;
			}

			// Ensure the buffers are null terminated to avoid MSVC-C6054.
			TraceBuffer[1023] = '\0';
			OutputBuffer[1023] = '\0';

			// Print to the console.
#	if IE_PLATFORM_BUILD_WIN32
			printf(TraceBuffer);
			printf(OutputBuffer);
			printf("\n");
			Insight::Debug::Logger::GetConsoleWindow().ResetColors();
#	elif IE_PLATFORM_BUILD_UWP
#		if _MSC_BUILD // Only Visual Studio can use 'OutputDebugString*'
			::OutputDebugStringA(TraceBuffer);
			::OutputDebugStringA(OutputBuffer);
			::OutputDebugStringA("\n");
#		endif // _MSC_BUILD
#	endif
		}

	} // end namespace Debug
} // end namespace Insight
