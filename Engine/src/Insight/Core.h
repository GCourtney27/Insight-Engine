#pragma once

#ifdef INSI_PLATFORM_WINDOW
	#ifdef INSI_BUILD_DLL
		#define INSIGHT_API __declspec(dllexport)
	#else
		#define INSIGHT_API __declspec(dllimport)
	#endif
#endif
