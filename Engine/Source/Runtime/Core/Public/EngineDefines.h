#pragma once

//// If we are compiling for Win32 or UWP we can assume our platform is Windows
//#if IE_PLATFORM_BUILD_WIN32 || IE_PLATFORM_BUILD_UWP
//#	define IE_PLATFORM_WINDOWS 1
//#endif
//
//// Only msvc supports __declspec
//#if IE_DYNAMIC_LINK
//#	if defined IE_PLATFORM_WINDOWS
//#		if defined IE_BUILD_DLL
//#			define INSIGHT_API __declspec (dllexport)
//#		else
//#			define INSIGHT_API __declspec (dllimport)
//#		endif // IE_PLATFORM_BUILD_WIN32
//#	endif
//#else
//#	define INSIGHT_API
//#endif
//
//#if defined IE_DEBUG
//#	define IE_ENABLE_ASSERTS
//#	define IE_SCOPE_PROFILING_ENABLED
//#endif // IE_DEBUG
//
//#if defined IE_ENABLE_ASSERTS
//#	define IE_ASSERT(Expr, ...) assert(Expr)
//#else
//#	define IE_ASSERT(Expr, ...)
//#endif // IE_ENABLE_ASSERTS
//
//// Configuration defines
//#if defined IE_GAME_DIST
//#	define IE_STRIP_FOR_GAME_DIST(x)
//#	define IE_ADD_FOR_GAME_DIST(x) x
//#else
//#	define IE_STRIP_FOR_GAME_DIST(x) x
//#	define IE_ADD_FOR_GAME_DIST(x)
//#endif
