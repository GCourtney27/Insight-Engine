#pragma once

/*
	Single include for any application utilizing the Insight Engine.
*/

// Core
#include "Insight/Core/Application.h"
#include "Insight/Core/Layer/Layer.h"
#if defined (IE_PLATFORM_BUILD_WIN32)
#include "Insight/Core/Layer/ImGuiOverlay.h"
#include "Platform/Win32/Win32Window.h"
#elif defined (IE_PLATFORM_BUILD_UWP)
#include "Platform/UWP/UWPWindow.h"
#endif

// Util
#include "Insight/Core/Log.h"
#include "Insight/Core/Exception.h"
#include "Insight/Utilities/Profiling.h"

// Rendering
#include "Insight/Rendering/Material.h"

// Game Framework
#include "Insight/Runtime/Components/StaticMeshComponent.h"



