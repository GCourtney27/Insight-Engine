#pragma once

/*
	Single include for any application utilizing the Insight Engine.
*/

// Core
#include "Runtime/Core/Application.h"
#include "Runtime/Core/Layer/Layer.h"
#if defined (IE_PLATFORM_BUILD_WIN32)
#include "Runtime/Core/Layer/ImGuiOverlay.h"
#include "Platform/Win32/Win32Window.h"
#elif defined (IE_PLATFORM_BUILD_UWP)
#include "Platform/UWP/UWPWindow.h"
#endif

// Util
#include "Runtime/Core/Log.h"
#include "Runtime/Core/Exception.h"
#include "Runtime/Utilities/Profiling.h"

// Rendering
#include "Runtime/Rendering/Material.h"

// Game Framework
#include "Runtime/GameFramework/Components/StaticMeshComponent.h"



