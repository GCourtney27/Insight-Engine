#include <Engine_pch.h>

#include "Log.h"


namespace Insight {

	namespace Debug {

		EString Logger::s_CoreDumpMessage;
		const TChar* Logger::s_LoggerName = TEXT("Insight Engine");

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

		void Logger::AppendMessageForCoreDump(const TChar* Message)
		{
			s_CoreDumpMessage.append(Message);
			s_CoreDumpMessage.append(TEXT("\n"));
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




		void LogHelper(ELogSeverity Severity, const TChar* fmt, const TChar* File, const TChar* Function, int Line, ...)
		{
			TChar TraceBuffer[1024];
			TChar OutputBuffer[1024];

			// Initialize the output message buffer.
			va_list args;
			va_start(args, Line); // Start capturing arguments after the 'Line' parameter in the method.
			{
				::_vsnwprintf(OutputBuffer, sizeof(OutputBuffer), fmt, args);
			}
			va_end(args);

			switch (Severity)
			{
			case ELogSeverity::Log:
				SET_CONSOLE_OUT_COLOR(CC_White);
				::swprintf_s(TraceBuffer, TEXT("[%s][Log] - "), Logger::GetLoggerName());
				break;
			case ELogSeverity::Verbose:
				SET_CONSOLE_OUT_COLOR(CC_Green);
				::swprintf_s(TraceBuffer, TEXT("[%s][Verbose][%s-%s-%i] - "), Logger::GetLoggerName(), File, Function, Line);
				break;
			case ELogSeverity::Warning:
				SET_CONSOLE_OUT_COLOR(CC_Yellow);
				::swprintf_s(TraceBuffer, TEXT("[%s][Warning] - "), Logger::GetLoggerName());
				break;
			case ELogSeverity::Error:
				SET_CONSOLE_OUT_COLOR(CC_Red);
				::swprintf_s(TraceBuffer, TEXT("[%s][Error] - "), Logger::GetLoggerName());
				break;
			case ELogSeverity::Critical:
				SET_CONSOLE_OUT_COLOR(CC_Orange);
				::swprintf_s(TraceBuffer, TEXT("[%s][Critical] - "), Logger::GetLoggerName());
				break;
			default:
				::swprintf_s(TraceBuffer, TEXT("Invalid log severity given to logger. Choose one option from ELogSeverity enum."));
				break;
			}

			// Ensure the buffers are null terminated to avoid MSVC-C6054.
			TraceBuffer[1023] = '\0';
			OutputBuffer[1023] = '\0';
			
			// Print to the console.
#	if IE_PLATFORM_BUILD_WIN32
			::wprintf(TraceBuffer);
			::wprintf(OutputBuffer);
			::wprintf(TEXT("\n"));
			// Log category may have changed the color, reset it back to the default.
			Insight::Debug::Logger::GetConsoleWindow().ResetColors();
#	elif IE_PLATFORM_BUILD_UWP
#		if _MSC_BUILD // Only Visual Studio can use 'OutputDebugString*'
			::OutputDebugString(TraceBuffer);
			::OutputDebugString(OutputBuffer);
			::OutputDebugString(TEXT("\n"));
#		endif // _MSC_BUILD
#	endif
		}

	} // end namespace Debug
} // end namespace Insight
