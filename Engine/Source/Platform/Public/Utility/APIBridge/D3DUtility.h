#pragma once

#include "EngineDefines.h"

#include "Platform/Public/Utility/COMException.h"
#include "Graphics/Public/CommonEnums.h"

namespace Insight
{
	namespace PlatformUtils
	{
#if IE_WINDOWS
		DXGI_FORMAT IETextureFormatToDXGIFormat(const Graphics::EFormat& Format);
		Graphics::EFormat DXGIFormatToIETextureFormat(DXGI_FORMAT Format);

		D3D12_COMMAND_LIST_TYPE IECommandListTypeToD3DCommandListType(const Graphics::ECommandListType& Type);
		Graphics::ECommandListType D3DCommandListTypeToIECommandListType(D3D12_COMMAND_LIST_TYPE Type);

		HRESULT CreateSwapChain(void* NativeWindow, const DXGI_SWAP_CHAIN_DESC1* Desc, BOOL AllowTearing, IDXGIFactory6** ppInFactory, IUnknown* pDevice, IDXGISwapChain3** ppOutSwapChain);

#endif
	}
}
