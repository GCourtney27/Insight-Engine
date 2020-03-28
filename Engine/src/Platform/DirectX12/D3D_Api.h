#pragma once

#include "Platform/Windows/Error/COM_Exception.h"

#include <wrl/client.h>
#include <windows.h>

// D3D12
#include <dxgi1_2.h>
#include "d3dx12.h"

/* ============================================
	
	This class is used as a helper to all applications 
	that rely on Direct3D as their renderer. It includes 
	helper funcitons and other useful items that each 
	D3D version can use.

 ===============================================*/

namespace Insight {

	using namespace Microsoft;

	#define COM_SAFE_RELEASE(p) { if( (p) ) { (p)->Release(); } }

	class D3DApi
	{
	public:
		D3DApi();
		~D3DApi();


	};

}