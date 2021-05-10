/*
	Contains macros to assist in debugging and other utilities.
*/
#pragma once


//////////////////////////////////
// Debug and Build Configurations
//////////////////////////////////

// Choose to enable optional debugging features.
#if ECS_DEBUG
#	define ECS_ENABLE_ASSERTS 1		// Enable assertion tests.
#	if ECS_DEVELOPMENT
#		define ECS_ENABLE_LOGGING 1 // Enable debug logging information from the ECS.
#	endif // ECS_DEVELOPMENT
#endif // ECS_DEBUG

#if ECS_ENABLE_LOGGING
#	include <ostream>
#	define DebugLog(Fmt, ...)		printf(Fmt, __VA_ARGS__)
#else
#	define DebugLog(Fmt, ...)
#endif // ECS_ENABLE_LOGGING

#if ECS_ENABLE_ASSERTS
#	include <assert.h>
#	define RuntimeAssert(Expr)		assert(Expr)
#else
#	define RuntimeAssert(Expr)
#endif // ECS_ENABLE_ASSERTS


//////////////////
// Utility Macros
//////////////////

#define ECS_NO_DISCARD				[[nodiscard]]
#define ECS_FORCE_INLINE			__forceinline
#define ECS_INLINE					inline
#define ZeroMemRanged(Dst, Size)	memset(Dst, 0, Size)
