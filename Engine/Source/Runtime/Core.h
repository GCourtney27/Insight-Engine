#pragma once

// If we are compiling for Win32 or UWP we can assume our platform is Windows
#if IE_PLATFORM_BUILD_WIN32 || IE_PLATFORM_BUILD_UWP
#	define IE_PLATFORM_WINDOWS 1
#endif

#if defined _MSC_VER
// Only MSVC supports __declspec
#	define DLL_EXPORT __declspec(dllexport)
#	define DLL_IMPORT __declspec(dllimport)
#else
#	define DLL_EXPORT
#	define DLL_IMPORT
#endif // _MSC_VER

#if IE_DYNAMIC_LINK
#	if defined (IE_PLATFORM_WINDOWS)
#		if defined IE_BUILD_DLL
#			define INSIGHT_API DLL_EXPORT
#		else
#			define INSIGHT_API DLL_IMPORT
#		endif // IE_PLATFORM_BUILD_WIN32
#	endif
#else
#	define INSIGHT_API
#endif

#if defined IE_DEBUG
#	define IE_ENABLE_ASSERTS 1
#	define IE_SCOPE_PROFILING_ENABLED 1
#	define IE_TRACK_RENDER_EVENTS 1
#else
#	define IE_ENABLE_ASSERTS 0
#	define IE_SCOPE_PROFILING_ENABLED 0
#	define IE_TRACK_RENDER_EVENTS 0
#endif // IE_DEBUG

#if IE_ENABLE_ASSERTS
#	define IE_ASSERT(Expr, ...) assert(Expr);
#else
#	define IE_ASSERT(Expr, ...)
#endif // IE_ENABLE_ASSERTS

// Configuration defines
#if defined IE_GAME_DIST
#	define IE_STRIP_FOR_GAME_DIST(x)
#	define IE_ADD_FOR_GAME_DIST(x) x
#else
#	define IE_STRIP_FOR_GAME_DIST(x) x
#	define IE_ADD_FOR_GAME_DIST(x)
#endif

// Includes
#include "Runtime/Core/Public/DataTypes.h"
#include "Runtime/Core/Public/EnumHelper.h"
#include "Runtime/Core/Public/Cast.h"
#include "Platform/Public/PlatformCommon.h"

#define NOMINMAX

// Utility Macros
#define BIT_SHIFT(x)				( 1 << x )
#define IE_BIND_LOCAL_EVENT_FN(Fn)	std::bind( &Fn, this, std::placeholders::_1 )
#define IE_BIND_EVENT_FN(Fn, Class) std::bind( &Fn, Class, std::placeholders::_1 )
#define IE_BIND_LOCAL_VOID_FN(Fn)	std::bind( &Fn, this )
#define SAFE_DELETE_PTR(Ptr)		if( (Ptr) ) { delete (Ptr); } else { IE_LOG(Error, TEXT("Trying to delete null pointer!")); }
#define COM_SAFE_RELEASE(ComObject) if( (ComObject) ) { (ComObject)->Release(); (ComObject) = nullptr; }
#define RAW_LITERAL(Value)			#Value
#define MACRO_TO_STRING(Macro)		RAW_LITERAL(Macro);
#define FORCE_INLINE				__forceinline
#define INLINE						inline 
#define NO_DISCARD					[[nodiscard]]
#define IE_ALIGN(InBytes)				__declspec( align(InBytes) )
#define NULL						0
#define IE_ARRAYSIZE(Arr)			( sizeof(Arr) / sizeof(Arr[0]) )
#define IE_MAX_PATH					260

template <typename T>
FORCE_INLINE constexpr void ZeroMem(T* Mem, size_t Size = sizeof(T))
{
	::memset(RCast<void*>(Mem), 0, Size);
}

// Rendering
#define MAX_POINT_LIGHTS_SUPPORTED 16u
#define MAX_DIRECTIONAL_LIGHTS_SUPPORTED 1u
#define MAX_SPOT_LIGHTS_SUPPORTED 16u

#define IE_CACHEOPTIMIZED_ECS_ENABLED 0
