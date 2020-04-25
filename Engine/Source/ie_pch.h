#pragma once

// === Standard Library ===
#include <memory>
#include <utility>
#include <iostream>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

// === Insight Specific ===
#include "Insight/Core/Log.h"

// === Windows Library ===
#ifdef IE_PLATFORM_WINDOWS
	// Windows API	
	#include <Windows.h>
	#include <wrl/client.h>

	// Direct3D 12 Includes
	#include <DirectXMath.h>
	#include <d3d12.h>
	#include <dxgi1_4.h>
	#include <D3Dcompiler.h>
	#include <dxgi1_2.h>
	#include <d3dx12.h>
	#include <wincodec.h>

#endif // IE_PLATFORM_WINDOWS