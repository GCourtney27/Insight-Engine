#pragma once
#include "Runtime/Graphics/Public/CommonEnums.h"
#include "Runtime/Graphics/Public/CommonStructs.h"

#if RENDER_CORE_BUILD_DLL
#	define RENDER_API __declspec(dllexport)
#else
#	define RENDER_API __declspec(dllimport)
#endif // RENDER_CORE

#define IE_SIMULTANEOUS_RENDER_TARGET_COUNT 8
#define IE_DEFAULT_STENCIL_READ_MASK 0xFF
#define IE_DEFAULT_STENCIL_WRITE_MASK 0xFF
#define IE_DEFAULT_DEPTH_BIAS 0
#define IE_DEFAULT_DEPTH_BIAS_CLAMP 0.0f
#define IE_DEFAULT_SLOPE_SCALED_DEPTH_BIAS 0.0f

namespace Insight
{
	namespace Graphics
	{
		// Forward Declares
		class IDevice;
		
		// Extern Variables
		extern class ICommandManager* g_pCommandManager;
		extern class IContextManager* g_pContextManager;
		extern class IDevice* g_pDevice;
	}
}
