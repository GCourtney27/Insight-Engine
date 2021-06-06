#pragma once



// Configuration defines
#if defined IE_GAME_DIST
#	define IE_STRIP_FOR_GAME_DIST(x)
#	define IE_ADD_FOR_GAME_DIST(x) x
#else
#	define IE_STRIP_FOR_GAME_DIST(x) x
#	define IE_ADD_FOR_GAME_DIST(x)
#endif


// Rendering
#define MAX_POINT_LIGHTS_SUPPORTED 16u
#define MAX_DIRECTIONAL_LIGHTS_SUPPORTED 1u
#define MAX_SPOT_LIGHTS_SUPPORTED 16u

