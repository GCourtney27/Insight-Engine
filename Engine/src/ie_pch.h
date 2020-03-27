#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

// TODO: implement log class
#include "Insight/Log.h"

#ifdef IE_PLATFORM_WINDOWS
	
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <Windows.h>

	// Direct3D 12 dependencies
	#include <d3d12.h>
	#include <dxgi1_4.h>
	#include <D3Dcompiler.h>
	#include <DirectXMath.h>

#endif // IE_PLATFORM_WINDOWS