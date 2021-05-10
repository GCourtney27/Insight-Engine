#pragma once

// If we are compiling for Win32 or UWP we can assume our platform is Windows
#if IE_PLATFORM_BUILD_WIN32 || IE_PLATFORM_BUILD_UWP
#	define IE_PLATFORM_WINDOWS 1
#endif

// Only msvc supports __declspec
#	if IE_DYNAMIC_LINK
#		if defined (IE_PLATFORM_WINDOWS)
#			if defined IE_BUILD_DLL
#				define INSIGHT_API __declspec(dllexport)
#			else
#				define INSIGHT_API __declspec(dllimport)
#			endif // IE_PLATFORM_BUILD_WIN32
#		endif
#	else
#		define INSIGHT_API
#	endif

#if defined IE_DEBUG
#	define IE_ENABLE_ASSERTS
#	define IE_SCOPE_PROFILING_ENABLED
#endif // IE_DEBUG

#if defined IE_ENABLE_ASSERTS
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
#include "Runtime/Math/Public/PrimitiveTypes.h"
#include "Runtime/Core/Public/EnumHelper.h"
#include "Runtime/Core/Public/Cast.h"
//#include "Runtime/Core/EString.h"
#include "Platform/Public/Common.h"

#define NOMINMAX

// Utilities
#define BIT_SHIFT(x) ( 1 << x )
#define IE_BIND_LOCAL_EVENT_FN(Fn) std::bind( &Fn, this, std::placeholders::_1 )
#define IE_BIND_EVENT_FN(Fn, Class) std::bind( &Fn, Class, std::placeholders::_1 )
#define IE_BIND_LOCAL_VOID_FN(Fn) std::bind( &Fn, this )
#define SAFE_DELETE_PTR(Ptr) if( (Ptr) ) { delete (Ptr); } else { IE_LOG(Warning, TEXT("Trying to delete null pointer!")) }
#define COM_SAFE_RELEASE(ComObject) if( (ComObject) ) { (ComObject)->Release(); (ComObject) = nullptr; }
#define RAW_LITERAL(Value) #Value
#define WIDE_STRING(Value) L#Value
#define MACRO_TO_STRING(Macro) RAW_LITERAL(Macro);
#define FORCE_INLINE __forceinline
#define INLINE inline 
#define NO_DISCARD [[nodiscard]]
#define ALIGN(InBytes) __declspec(align(InBytes))
template <typename T>
FORCE_INLINE constexpr void ZeroMem(T* Mem, size_t Size = sizeof(T))
{
	::memset(RCast<void*>(Mem), 0, Size);
}

// Rendering
#define MAX_POINT_LIGHTS_SUPPORTED 16u
#define MAX_DIRECTIONAL_LIGHTS_SUPPORTED 1u
#define MAX_SPOT_LIGHTS_SUPPORTED 16u
