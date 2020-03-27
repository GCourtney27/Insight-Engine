#pragma once
#include <wrl/client.h>
#include <windows.h>
#include <wrl/client.h>

#include <dxgi1_2.h>
#include "d3dx12.h"

namespace Insight {

	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw std::exception();
		}
	}

	class DXApp
	{
	public:
		DXApp();
		~DXApp();

		//void GetHardwareAdapters(_In_ IDXGIFactory2* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** pAdapter);;

	};

}