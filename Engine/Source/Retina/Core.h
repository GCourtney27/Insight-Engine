#pragma once

#if defined RN_PLATFORM_WINDOWS
	#if defined RN_DYNAMIC_LINK
		#if defined RN_BUILD_DLL
			#define RETINA_API __declspec(dllexport)
		#else
			#define RETINA_API __declspec(dllimport)
		#endif
	#else
		#define RETINA_API 
	#endif
#endif // RN_PLATFORM_WINDOWS

#if defined RN_ENGINE_DIST || defined RN_GAME_DIST
	#define RN_PRODUCTION
#endif

#if defined RN_DEBUG
	#define RN_ENABLE_ASSERTS
	#define RN_SCOPE_PROFILING_ENABLED
#endif // RN_DEBUG

#if defined RN_ENABLE_ASSERTS
	#define RN_ASSERT(x, ...) {if( !(x) ) { RN_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define RN_CORE_ASSERT(x, ...) { if(!(x)) { RN_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define RN_ASSERT(x, ...)
	#define RN_CORE_ASSERT(x, ...)
#endif // RN_ENABLE_ASSERTS

// Configuration defines
#if defined RN_GAME_DIST
#define RN_STRIP_FOR_GAME_DIST(x)
#define RN_ADD_FOR_GAME_DIST(x) x
#else
#define RN_STRIP_FOR_GAME_DIST(x) x
#define RN_ADD_FOR_GAME_DIST(x)
#endif

// Utilities
#define BIT_SHIFT(x) ( 1 << x )
#define RN_BIND_EVENT_FN(fn) std::bind( &fn, this, std::placeholders::_1 )
#define COM_SAFE_RELEASE(ptr) if(ptr) ptr->Release();
#define CAST_TO_FLOAT(...) static_cast<float>(__VA_ARGS__)
#define CAST_TO_UINT(...) static_cast<UINT>(__VA_ARGS__)
#define QUOTE(value) #value
#define MACRO_TO_STRING(macro) QUOTE(macro);

// Includes
#include "Retina/Math/Math_Helpers.h"

// Rendering
#define MAX_POINT_LIGHTS_SUPPORTED 16U
#define MAX_DIRECTIONAL_LIGHTS_SUPPORTED 1U
#define MAX_SPOT_LIGHTS_SUPPORTED 16U
