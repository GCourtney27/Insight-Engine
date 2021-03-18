#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/EString.h"
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
			static void AppendMessageForCoreDump(const TChar* Message);
			
			/*
				Begins the core dump and writs to a file.
			*/
			static void InitiateCoreDump();

			static inline const TChar* GetLoggerName() { return s_LoggerName; }

#	if (IE_DEBUG) && defined (IE_PLATFORM_BUILD_WIN32)
			/*
				Returns a reference to the currently active console window.
			*/
			inline static ConsoleWindow& GetConsoleWindow() { return s_ConsoleWindow; }
#endif

		private:
			static const TChar* s_LoggerName;
			static EString s_CoreDumpMessage;
#	if defined (IE_DEBUG) && defined (IE_PLATFORM_BUILD_WIN32)
			static ConsoleWindow s_ConsoleWindow;
#	endif
		};



		// Logger that will log output to the console window. Recommended that you dont call directly. 
		// Instead, use IE_LOG so logs will be stripped from release builds.
		void LogHelper(ELogSeverity Severity, const TChar* fmt, const TChar* File, const TChar* Function, int Line, ...);

	} // end namespace Debug
} // end namespace Insight

#if IE_DEBUG

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
#	define IE_LOG(Severity, fmt, ...) ::Insight::Debug::LogHelper(ELogSeverity::##Severity, fmt, __FILEW__, __FUNCTIONW__, __LINE__, __VA_ARGS__);

#if IE_PLATFORM_BUILD_WIN32
#	define SET_CONSOLE_OUT_COLOR(Color) ::Insight::Debug::Logger::GetConsoleWindow().SetForegroundColor(EConsoleColor::##Color)
#else
#	define SET_CONSOLE_OUT_COLOR(Color)
#endif 


#else
#	define IE_FATAL_ERROR(...)
#	define IE_LOG(LogSeverity, fmt, ...)
#	define SET_CONSOLE_OUT_COLOR(Color)
#endif
