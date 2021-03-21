#include <Engine_pch.h>

#include "Platform/DirectX12/Private/D3D12SwapChain.h"
#include "Platform/DirectX12/Private/D3D12CommandManager.h"

#include "Platform/Win32/Error/COMException.h"


namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			
			D3D12SwapChain::D3D12SwapChain()
				: m_pDXGISwapChain(NULL)
				, m_Desc()
			{
			}
			
			D3D12SwapChain::~D3D12SwapChain()
			{
			}
			
			void D3D12SwapChain::SwapBuffers()
			{
				UInt32 PresetFlags = ( (m_Desc.Flags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING) && m_FullScreenEnabled )
					? DXGI_PRESENT_ALLOW_TEARING : 0;
				m_pDXGISwapChain->Present( SCast<UInt32>(m_VSyncEnabled), PresetFlags );
			}

			void D3D12SwapChain::Resize()
			{
			}

			void D3D12SwapChain::ToggleFullScreen(bool IsEnabled)
			{
				ISwapChain::ToggleFullScreen(IsEnabled);

			}

			void D3D12SwapChain::Initialize(const IESwapChainCreateDesc& InitParams, IDXGIFactory6** ppDXGIFactory, D3D12CommandQueue* ppCommandQueue)
			{
				IE_ASSERT(ppCommandQueue != NULL);
				IE_ASSERT(ppDXGIFactory != NULL);

				ID3D12CommandQueue* pCommandQueue = RCast<ID3D12CommandQueue*>(ppCommandQueue->GetNativeQueue());
				IE_ASSERT(pCommandQueue != NULL);
				
				DXGI_SAMPLE_DESC SampleDesc;
				ZeroMem(&SampleDesc);
				SampleDesc.Count = 1;
				SampleDesc.Quality = 0;

				DXGI_SWAP_CHAIN_DESC1 Desc;
				ZeroMem(&Desc);
				Desc.Width			= InitParams.Width;
				Desc.Height			= InitParams.Height;
				Desc.BufferCount	= InitParams.BufferCount;
				Desc.SampleDesc		= SampleDesc;
				Desc.SwapEffect		= DXGI_SWAP_EFFECT_FLIP_DISCARD;
				Desc.BufferUsage	= DXGI_USAGE_RENDER_TARGET_OUTPUT;
				Desc.Format			= PlatformHelpers::IEFormatToDXGIFormat(InitParams.Format);
				Desc.Flags			= InitParams.AllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

				HRESULT hr = S_OK;
				IDXGISwapChain1* pTempSwapChain = NULL;
#if IE_PLATFORM_BUILD_WIN32
				hr = (*ppDXGIFactory)->CreateSwapChainForHwnd(
					pCommandQueue,
					SCast<HWND>(InitParams.NativeWindow),
					&Desc,
					nullptr,
					nullptr,
					&pTempSwapChain
				);
				ThrowIfFailed(hr, TEXT("Failed to create swap chain for Win32 window!"));
				if (InitParams.AllowTearing)
				{
					ThrowIfFailed((*ppDXGIFactory)->MakeWindowAssociation(SCast<HWND>(InitParams.NativeWindow), DXGI_MWA_NO_ALT_ENTER)
						, TEXT("Failed to Make Window Association"));
				}
#elif IE_PLATFORM_BUILD_UWP
				hr = (*ppDXGIFactory)->CreateSwapChainForCoreWindow(
					pCommandQueue,
					RCast<::IUnknown*>(InitParams.NativeWindow),
					&Desc,
					nullptr,
					&pTempSwapChain
				);
				ThrowIfFailed(hr, TEXT("Failed to Create swap chain for UWP window!"));
#endif

				ThrowIfFailed(pTempSwapChain->QueryInterface(IID_PPV_ARGS(&m_pDXGISwapChain))
					, TEXT("Failed to query interface for temporary DXGI swapchain!"));

				SetCurrentFrameIndex(m_pDXGISwapChain->GetCurrentBackBufferIndex());

				m_Desc = Desc;
			}
			
			void D3D12SwapChain::UnInitialize()
			{
			}
		}
	}
}
