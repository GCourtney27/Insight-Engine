#include <Engine_pch.h>

#if IE_PLATFORM_BUILD_WIN32 && IE_PLATFORM_BUILD_UWP
#	error [INSIGHT ERROR] - Cannot build for Win32 or UWP platforms with current startup project and configuration.
#endif

#if IE_PLATFORM_BUILD_XBOX_ONE && IE_DEBUG && IE_WITH_EDITOR
#	error [INSIGHT ERROR] - Cannot build debug editor with Xbox One configuration active.
#endif

#if IE_PLATFORM_BUILD_XBOX_ONE && IE_PLATFORM_BUILD_WIN32
#	error [INSIGHT ERROR] - Cannot build for Xbox One platform while Win32 project is selected.
#endif


