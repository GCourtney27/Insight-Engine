#pragma once
#include "Runtime/Graphics/Public/CommonEnums.h"
#include "Runtime/Graphics/Public/CommonStructs.h"

#if RENDER_CORE_BUILD_DLL
#	define RENDER_API __declspec(dllexport)
#else
#	define RENDER_API __declspec(dllimport)
#endif // RENDER_CORE

namespace Insight
{
	namespace Graphics
	{
		// Forward Declares
		class IDevice;
		
		// Extern Variables
		extern class ICommandManager* g_pCommandManager;
		extern class IContextManager* g_pContextManager;
	}
}
