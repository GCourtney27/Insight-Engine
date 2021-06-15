#pragma once

/*
	Single include for any application utilizing the Insight Engine.
*/

// -----------------------------------------------------
// C++ library headers that are needed by the engine.	|
// -----------------------------------------------------
#include <map>
#include <array>
#include <stack>
#include <queue>
#include <vector>
#include <future>
#include <string>
#include <math.h>
#include <thread>
#include <memory>
#include <sstream>
#include <utility>
#include <fstream>
#include <assert.h>
#include <iostream>
#include <typeinfo>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <type_traits>
#include <string_view>
#include <xmmintrin.h>
#include <unordered_map>
#include <unordered_set>

// -----
// Core	|
// -----
#include "Core/Public/Engine.h"
#include "Platform/Public/System.h"
#include "Core/Public/Layer/Layer.h"
#if IE_WIN32
#	include "Core/Public/Layer/ImGuiOverlay.h"
#	include "Platform/Win32/Win32Window.h"
#elif IE_UWP_DESKTOP
#	include "Platform/UWP/UWPWindow.h"
#endif

// ---------
// Utility	|
// ---------

#include "Core/Public/Log.h"
#include "Core/Public/Exception.h"
#include "Core/Public/Utility/Profiling.h"

// ------------
// Rendering	|
// ------------
#include "Graphics/Material.h"

// -----------------
// Game Framework	|
// -----------------
#include "GameFramework/Components/StaticMeshComponent.h"

using namespace Insight;

