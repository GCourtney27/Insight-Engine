#ifndef ENGINE_DEFINES
#define ENGINE_DEFINES


#define NOMINMAX


// If we are compiling for Win32 or UWP we can assume our platform is Windows.
#if IE_WIN32 || IE_UWP_DESKTOP
#	define IE_WINDOWS 1
#endif
#if IE_UWP_DESKTOP || IE_XBOX_ONE || IE_XBOX_ONE_X
#	define IE_UNIVERSAL_WINDOWS 1
#endif


#if INSIGHT_ENGINE

#	define IE_CACHEOPTIMIZED_ECS_ENABLED 0

#	if defined _MSC_VER
		// Only MSVC supports __declspec
#		define DLL_EXPORT __declspec(dllexport)
#		define DLL_IMPORT __declspec(dllimport)
#	else
#		define DLL_EXPORT
#		define DLL_IMPORT
#	endif // _MSC_VER

#	if IE_DYNAMIC_LINK
#		if defined (IE_WINDOWS)
#			if defined IE_BUILD_DLL
#				define INSIGHT_API DLL_EXPORT
#			else
#				define INSIGHT_API DLL_IMPORT
#			endif // IE_WIN32
#		endif
#	else
#		define INSIGHT_API
#	endif


#	if IE_DEBUG
#		define IE_ENABLE_ASSERTS 1
#		define IE_SCOPE_PROFILING_ENABLED 1
#		define IE_TRACK_RENDER_EVENTS 1
#	else
#		define IE_ENABLE_ASSERTS 0
#		define IE_SCOPE_PROFILING_ENABLED 0
#		define IE_TRACK_RENDER_EVENTS 0
#	endif // IE_DEBUG

#	if IE_ENABLE_ASSERTS
#		define IE_ASSERT(Expr, ...) assert(Expr);
#	else
#		define IE_ASSERT(Expr, ...)
#	endif // IE_ENABLE_ASSERTS

// TODO Depricate!
#define IE_BIND_LOCAL_EVENT_FN(Fn)	std::bind( &Fn, this, std::placeholders::_1 )
#define IE_BIND_EVENT_FN(Fn, Class) std::bind( &Fn, Class, std::placeholders::_1 )
#define IE_BIND_LOCAL_VOID_FN(Fn)	std::bind( &Fn, this )


/////////////////////////////////////////////
// Platform and Configurations Strings
/////////////////////////////////////////////

#if IE_WIN32
#	define IE_PLATFORM_STRING	"Win32 Desktop"
#elif IE_UWP_DESKTOP 
#	define IE_PLATFORM_STRING	"Universal Windows Desktop"
#elif IE_XBOX_ONE 
#	define IE_PLATFORM_STRING	"Xbox One"
#elif IE_XBOX_ONE_X
#	define IE_PLATFORM_STRING	"Xbox One X"
#else
#	message Unrecognized platform provided when determining platform string!
#endif

#if IE_DEBUG_EDITOR
#	define IE_CONFIG_STRING "Debug Editor"
#elif IE_DEVELOPMENT
#	define IE_CONFIG_STRING "Development"
#elif IE_DEBUG_GAME
#	define IE_CONFIG_STRING "Pre-Release"
#elif IE_SHIPPING
#	define IE_CONFIG_STRING "Shipping"
#else
#	message Unrecognized configuration when determining config string!
#endif

#endif // INSIGHT_ENGINE


//////////////////////////
//		Entry Point	
//////////////////////////

#if IE_WIN32 || IE_UWP_DESKTOP || IE_XBOX_ONE || IE_XBOX_ONE_X
// Win32 Api
#	define IE_ENTRY_POINT						\
	int APIENTRY wWinMain(						\
		_In_		HINSTANCE hInstance,		\
		_In_opt_	HINSTANCE hPrevInstance,	\
		_In_		LPWSTR lpCmdLine,			\
		_In_		int nCmdShow)					
#endif

// Rendering
//
#define IE_MAX_SPOT_LIGHTS			4
#define IE_MAX_POINT_LIGHTS			4
#define IE_MAX_DIRECTIONAL_LIGHTS	4

#define kSceneConstantsReg  b0
#define kMeshWorldReg       b1
#define kMaterialReg        b2
#define kLightsReg          b3

#endif // ENGINE_DEFINES