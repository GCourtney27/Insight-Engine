#pragma once

#include <Runtime/CoreMacros.h>

#if defined IE_PLATFORM_BUILD_WIN32
#	include "Platform/Win32/ConsoleWindow.h"
#endif 


/*
	Valid log categories for IE_LOG
*/
enum class ELogSeverity
{
	Log,
	Verbose,
	Warning,
	Error,
	Critical,
};

namespace Insight {

	namespace Debug {


		class INSIGHT_API Logger
		{
		public:
			Logger() = default;
			~Logger();
			
			/*
				Initialize the console window and other debug utilities for logging. Returns 
				true if succeeded, false if not.
			*/
			static bool Initialize();

			/*
				Appends a message to be dumped to log file.
				@param Message - The message to be written to the dump file.
			*/
			inline static void AppendMessageForCoreDump(const char* Message)
			{
				s_CoreDumpMessage.append(Message);
				s_CoreDumpMessage.append("\n");
			}

			/*
				Begins the core dump and writs to a file.
			*/
			static void InitiateCoreDump();

#	if defined (IE_DEBUG) && defined (IE_PLATFORM_BUILD_WIN32)
			/*
				Returns a reference to the currently active console window.
			*/
			inline static ConsoleWindow& GetConsoleWindow() { return s_ConsoleWindow; }
#endif

		private:
			static std::string s_CoreDumpMessage;
#	if defined (IE_DEBUG) && defined (IE_PLATFORM_BUILD_WIN32)
			static ConsoleWindow s_ConsoleWindow;
#	endif
		};



		// Logger that will log output to the console window. Recommended that you dont call directly. 
		// Instead, use IE_LOG so logs will be stripped from release builds.
		void LogHelper(ELogSeverity Severity, const char* fmt, const char* File, const char* Function, int Line, ...);

	} // end namespace Debug
} // end namespace Insight

#if defined (IE_DEBUG) || defined (IE_RELEASE)

/*
	If Expr is true, a exception will be raised.
	@param Expr - The expression to break on if evaluated to false.
	@param Message - A helpful message that can be written to some debug display. If exception is not caught it will be dumped to a file.
	@param Category - The high level category of the exception.
*/
#	define IE_FATAL_ERROR(Expr, Message, Category) if( !(Expr) ) { throw ::Insight::ieException(Message, Category); }

/*
	Log a message to the console.
	@param Severety - The severity of the error.
	@param fmt - the format to display when writing to the console.
	@param ... - Optional arguments to supply when printing.
*/
#	define IE_LOG(Severity, fmt, ...) ::Insight::Debug::LogHelper(ELogSeverity::##Severity, fmt, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

#else
#	define IE_FATAL_ERROR(...)
#	define IE_LOG(LogSeverity, fmt, ...)
#endif
