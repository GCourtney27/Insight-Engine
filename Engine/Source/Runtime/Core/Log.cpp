#include <Engine_pch.h>

#include "Log.h"


namespace Insight {

	namespace Debug {

		std::string Logger::s_CoreDumpMessage;
		const char* Logger::s_LoggerName = "Insight Engine";

#if defined IE_DEBUG && defined IE_PLATFORM_BUILD_WIN32
		ConsoleWindow Logger::s_ConsoleWindow;
#endif

		bool Logger::Initialize()
		{
#	if defined (IE_DEBUG) && defined (IE_PLATFORM_BUILD_WIN32)
			// Initialize the console window.
			ConsoleWindowDesc WindowDesc;
			WindowDesc.CanClose = false;
			WindowDesc.BufferDims = ieFloat2(700, 320);
			WindowDesc.WindowDims = ieFloat2(170, 42);
			WindowDesc.DefaultForegroundColor = CC_White;
			WindowDesc.LoggerName = "Insight";
			s_ConsoleWindow.Create(WindowDesc);
#endif

			return true;
		}

		Logger::~Logger()
		{
		}

		void Logger::InitiateCoreDump()
		{
			// Assemble the data.
			time_t RawTime;
			struct tm* TimeInfo;
			char DateBuffer[80];
			time(&RawTime);
			TimeInfo = localtime(&RawTime);
			strftime(DateBuffer, 80, "Date: %y-%m-%d  Time: %I:%M%p", TimeInfo);
			
			// Write the message to the dump file.
			std::ofstream OFStream;
			OFStream.open("CoreDump.txt");
			OFStream << "Insight Engine Core Dump\n";
			OFStream << DateBuffer;
			OFStream << "\n";
			OFStream << s_CoreDumpMessage.c_str();
			OFStream.close();
		}




		void LogHelper(ELogSeverity Severity, const char* fmt, const char* File, const char* Function, int Line, ...)
		{
			char TraceBuffer[1024];
			char OutputBuffer[1024];

			// Initialize the output message buffer.
			va_list args;
			va_start(args, Line); // Start capturing arguments after the 'Line' parameter in the method.
			{
				::_vsnprintf(OutputBuffer, sizeof(OutputBuffer), fmt, args);
			}
			va_end(args);

			switch (Severity)
			{
			case ELogSeverity::Log:
				SET_CONSOLE_OUT_COLOR(CC_White);
				::sprintf_s(TraceBuffer, "[%s][Log] - ", Logger::GetLoggerName());
				break;
			case ELogSeverity::Verbose:
				SET_CONSOLE_OUT_COLOR(CC_Green);
				::sprintf_s(TraceBuffer, "[%s][Verbose][%s-%s-%i] - ", Logger::GetLoggerName(), File, Function, Line);
				break;
			case ELogSeverity::Warning:
				SET_CONSOLE_OUT_COLOR(CC_Yellow);
				::sprintf_s(TraceBuffer, "[%s][Warning] - ", Logger::GetLoggerName());
				break;
			case ELogSeverity::Error:
				SET_CONSOLE_OUT_COLOR(CC_Red);
				::sprintf_s(TraceBuffer, "[%s][Error] - ", Logger::GetLoggerName());
				break;
			case ELogSeverity::Critical:
				SET_CONSOLE_OUT_COLOR(CC_Orange);
				::sprintf_s(TraceBuffer, "[%s][Critical] - ", Logger::GetLoggerName());
				break;
			default:
				::sprintf_s(TraceBuffer, "Invalid log severity given to logger. Choose one option from ELogSeverity enum.");
				break;
			}

			// Ensure the buffers are null terminated to avoid MSVC-C6054.
			TraceBuffer[1023] = '\0';
			OutputBuffer[1023] = '\0';

			// Print to the console.
#	if IE_PLATFORM_BUILD_WIN32
			::printf(TraceBuffer);
			::printf(OutputBuffer);
			::printf("\n");
			// Log category may have changed the color, reset it back to the default.
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
