#pragma once

#if defined IE_PLATFORM_WINDOWS
	#if defined IE_DYNAMIC_LINK
		#if defined IE_BUILD_DLL
			#define INSIGHT_API __declspec(dllexport)
		#else
			#define INSIGHT_API __declspec(dllimport)
		#endif
	#else
		#define INSIGHT_API 
	#endif
#endif // IE_PLATFORM_WINDOWS

#if defined IE_ENGINE_DIST || defined IE_GAME_DIST
	#define IE_PRODUCTION
#endif

#if defined IE_DEBUG
	#define IE_ENABLE_ASSERTS
	#define IE_SCOPE_PROFILING_ENABLED
#endif // IE_DEBUG

#if defined IE_ENABLE_ASSERTS
	#define IE_ASSERT(x, ...) {if( !(x) ) { IE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define IE_CORE_ASSERT(x, ...) { if(!(x)) { IE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define IE_ASSERT(x, ...)
	#define IE_CORE_ASSERT(x, ...)
#endif // IE_ENABLE_ASSERTS

// Configuration defines
#if defined IE_GAME_DIST
#define IE_STRIP_FOR_GAME_DIST(x)
#define IE_ADD_FOR_GAME_DIST(x) x
#else
#define IE_STRIP_FOR_GAME_DIST(x) x
#define IE_ADD_FOR_GAME_DIST(x)
#endif

// Utilities
#define BIT_SHIFT(x) ( 1 << x )
#define IE_BIND_EVENT_FN(fn) std::bind( &fn, this, std::placeholders::_1 )
#define COM_SAFE_RELEASE(ptr) if(ptr) ptr->Release();
#define CAST_TO_FLOAT(...) static_cast<float>(__VA_ARGS__)
#define CAST_TO_UINT(...) static_cast<UINT>(__VA_ARGS__)
#define QUOTE(value) #value
#define MACRO_TO_STRING(macro) QUOTE(macro);
#define FORCE_INLINE __forceinline

// Includes
#include "Insight/Math/Math_Helpers.h"

// Rendering
#define MAX_POINT_LIGHTS_SUPPORTED 16U
#define MAX_DIRECTIONAL_LIGHTS_SUPPORTED 1U
#define MAX_SPOT_LIGHTS_SUPPORTED 16U
