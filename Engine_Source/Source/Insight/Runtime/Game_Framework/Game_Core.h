#pragma once

// Internal naming convention for the game's runtime.
// DO NOT CHANGE!
#define InsightGame GameInternal

// Determine if the IGame framework should import of export the symbols.
#if defined (BUILD_GAME_DLL)
#	if defined (COMPILE_DLL)
#		define GAME_API __declspec(dllexport)
#		include "Insight/Runtime/Game_Framework/IGame.h"
#	else
#		define GAME_API __declspec(dllimport)
#	endif // COMPILE_DLL
#else
#	define GAME_API
#endif // BUILD_GAME_DLL
