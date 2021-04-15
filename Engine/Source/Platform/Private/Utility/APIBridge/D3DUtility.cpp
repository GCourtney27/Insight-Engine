#include <Engine_pch.h>

#include "Platform/Public/Utility/APIBridge/D3DUtility.h"

namespace Insight
{
	namespace PlatformUtils
	{


		// ----------------------------------
		//		Texture Type conversions
		// ----------------------------------

		DXGI_FORMAT IETextureFormatToDXGIFormat(const Graphics::EFormat& Format)
		{
			switch (Format)
			{
			case Graphics::EFormat::F_R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
			default:
				IE_LOG(Error, TEXT("Invalid format trying to convert ETextureFormat to DXGI_FORMAT!"));
				return DXGI_FORMAT_B8G8R8A8_TYPELESS;
			}

		}

		Graphics::EFormat DXGIFormatToIETextureFormat(DXGI_FORMAT Format)
		{
			switch (Format)
			{
			case DXGI_FORMAT_R8G8B8A8_UNORM: return Graphics::EFormat::F_R8G8B8A8_UNORM;
			default:
				IE_LOG(Error, TEXT("Invalid format trying to convert DXGI_FORMAT to ETextureFormat!"));
				return Graphics::EFormat::F_R8G8B8A8_UNORM;
			}
		}


		// -------------------------------------
		//	D3D12 Command List Type Conversions
		// -------------------------------------

		D3D12_COMMAND_LIST_TYPE IECommandListTypeToD3DCommandListType(const Graphics::ECommandListType& Type)
		{
			switch (Type)
			{
			case Graphics::ECommandListType::CLT_Direct: return D3D12_COMMAND_LIST_TYPE_DIRECT;
			case Graphics::ECommandListType::CLT_Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
			default:
				IE_LOG(Error, TEXT("Failed to convert ECommandListType to D3D12_COMMAND_LIST_TYPE with specified enum value!"));
				return D3D12_COMMAND_LIST_TYPE_DIRECT;
			}
		}

		Graphics::ECommandListType D3DCommandListTypeToIECommandListType(D3D12_COMMAND_LIST_TYPE Type)
		{
			switch (Type)
			{
			case D3D12_COMMAND_LIST_TYPE_DIRECT: return Graphics::ECommandListType::CLT_Direct;
			case D3D12_COMMAND_LIST_TYPE_COMPUTE: return Graphics::ECommandListType::CLT_Compute;
			default:
				IE_LOG(Error, TEXT("Failed to convert D3D12_COMMAND_LIST_TYPE to ECommandListType with specified enum value!"));
				return Graphics::ECommandListType::CLT_Direct;
			}
		}

		HRESULT CreateSwapChain(void* NativeWindow, const DXGI_SWAP_CHAIN_DESC1* Desc, BOOL AllowTearing, IDXGIFactory6** ppInFactory, IUnknown* pDevice, IDXGISwapChain3** ppOutSwapChain)
		{
			HRESULT hr = S_OK;
			IDXGISwapChain1* pTempSwapChain = NULL;
#if IE_PLATFORM_BUILD_WIN32
			hr = (*ppInFactory)->CreateSwapChainForHwnd(pDevice, SCast<HWND>(NativeWindow), Desc, NULL, NULL, &pTempSwapChain);
			ThrowIfFailed(hr, TEXT("Failed to create swap chain for HWND!"));
			if (AllowTearing)
			{
				ThrowIfFailed((*ppInFactory)->MakeWindowAssociation(SCast<HWND>(NativeWindow), DXGI_MWA_NO_ALT_ENTER)
					, TEXT("Failed to Make Window Association"));
			}
#elif IE_PLATFORM_BUILD_UWP
			hr = (*ppInFactory)->CreateSwapChainForCoreWindow(pDevice, RCast<::IUnknown*>(NativeWindow), Desc, NULL, &pTempSwapChain);
			ThrowIfFailed(hr, TEXT("Failed to Create swap chain for CoreWindow!"));
#endif

			ThrowIfFailed(pTempSwapChain->QueryInterface(IID_PPV_ARGS(ppOutSwapChain))
				, TEXT("Failed to query interface for temporary DXGI swapchain!"));

			return hr;
		}

	}
}
