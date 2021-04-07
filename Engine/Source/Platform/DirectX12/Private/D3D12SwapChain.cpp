#include <Engine_pch.h>

#include "Platform/DirectX12/Private/D3D12SwapChain.h"

#include "Platform/DirectX12/Public/Resource/D3D12ColorBuffer.h"
#include "Platform/Public/Utility/APIBridge/D3DUtility.h"
#include "Platform/DirectX12/Private/D3D12CommandManager.h"
#include "Platform/DirectX12/Private/D3D12CommonGlobals.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			
			D3D12SwapChain::D3D12SwapChain()
				: m_pID3D12DeviceRef(NULL)
				, m_pDXGISwapChain(NULL)
			{
			}
			
			D3D12SwapChain::~D3D12SwapChain()
			{
				UnInitialize();
			}

			void D3D12SwapChain::Initialize(IDevice* pDevice)
			{
				m_pDeviceRef = pDevice;
			}

			void D3D12SwapChain::Create(const IESwapChainDesc& InitParams, IDXGIFactory6** ppDXGIFactory, D3D12CommandQueue* ppCommandQueue, ID3D12Device* pDevice)
			{
				IE_ASSERT(ppCommandQueue != NULL);
				IE_ASSERT(ppDXGIFactory != NULL);
				IE_ASSERT(ppDXGIFactory != NULL);
				IE_ASSERT(pDevice != NULL);

				ID3D12CommandQueue* pCommandQueue = RCast<ID3D12CommandQueue*>(ppCommandQueue->GetNativeQueue());
				IE_ASSERT(pCommandQueue != NULL);

				m_pID3D12DeviceRef = pDevice;
				m_Desc = InitParams;
				//m_DisplayPlanes.reserve(InitParams.BufferCount);
				for (UInt64 i = 0; i < InitParams.BufferCount; ++i)
					m_DisplayPlanes.push_back(new D3D12ColorBuffer());

				DXGI_SAMPLE_DESC SampleDesc;
				ZeroMem(&SampleDesc);
				SampleDesc.Count = 1;
				SampleDesc.Quality = 0;

				DXGI_SWAP_CHAIN_DESC1 Desc;
				ZeroMem(&Desc);
				Desc.Width = InitParams.Width;
				Desc.Height = InitParams.Height;
				Desc.BufferCount = InitParams.BufferCount;
				Desc.SampleDesc = SampleDesc;
				Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
				Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				Desc.Format = PlatformUtils::IETextureFormatToDXGIFormat(InitParams.Format);
				Desc.Flags = InitParams.AllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

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
				ThrowIfFailed(hr, TEXT("Failed to create swap chain for HWND!"));
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
				ThrowIfFailed(hr, TEXT("Failed to Create swap chain for CoreWindow!"));
#endif

				ThrowIfFailed(pTempSwapChain->QueryInterface(IID_PPV_ARGS(&m_pDXGISwapChain))
					, TEXT("Failed to query interface for temporary DXGI swapchain!"));

				SetCurrentFrameIndex(m_pDXGISwapChain->GetCurrentBackBufferIndex());

				BindSwapChainBackBuffers();
			}

			void D3D12SwapChain::UnInitialize()
			{
				COM_SAFE_RELEASE(m_pDXGISwapChain);
			}
			
			void D3D12SwapChain::SwapBuffers()
			{
				UInt32 PresetFlags = ( m_Desc.AllowTearing && m_bFullScreenEnabled )
					? DXGI_PRESENT_ALLOW_TEARING : 0;
				m_pDXGISwapChain->Present( SCast<UInt32>(m_bVSyncEnabled), PresetFlags );

				MoveToNextFrame();
			}

			void D3D12SwapChain::Resize(const FVector2& NewResolution)
			{
				m_Desc.Width = SCast<UInt32>(NewResolution.x);
				m_Desc.Height = SCast<UInt32>(NewResolution.y);
				ResizeDXGIBuffers();
			}

			void D3D12SwapChain::SetNumBackBuffes(UInt32 NumBuffers)
			{
				IE_ASSERT(NumBuffers <= DXGI_MAX_SWAP_CHAIN_BUFFERS);

				m_Desc.BufferCount = NumBuffers;
				ResizeDXGIBuffers();
			}

			void D3D12SwapChain::SetBackBufferFormat(ETextureFormat& Format)
			{
				m_Desc.Format = Format;
				ResizeDXGIBuffers();
			}

			void D3D12SwapChain::ToggleFullScreen(bool IsEnabled)
			{
				ISwapChain::ToggleFullScreen(IsEnabled);

				HRESULT hr = S_OK;
				BOOL FullScreenState;
				hr = m_pDXGISwapChain->GetFullscreenState(&FullScreenState, NULL);
				ThrowIfFailed(hr, TEXT("Failed to get full screen state for swap chain!"))

				if (IsEnabled && FullScreenState)
				{
					IE_LOG(Warning, TEXT("Full screen state is already active."));
					return;
				}
				
				hr = m_pDXGISwapChain->SetFullscreenState(!FullScreenState, NULL);
				ThrowIfFailed(hr, TEXT("Failed to set fullscreen state for swap chain!"));
			}

			void D3D12SwapChain::ResizeDXGIBuffers()
			{
				DXGI_FORMAT Format = PlatformUtils::IETextureFormatToDXGIFormat(m_Desc.Format);
				DXGI_SWAP_CHAIN_DESC1 DXGIDesc = { 0 };
				m_pDXGISwapChain->GetDesc1(&DXGIDesc);

				HRESULT hr = m_pDXGISwapChain->ResizeBuffers(m_Desc.BufferCount, m_Desc.Width, m_Desc.Height, Format, DXGIDesc.Flags);
				ThrowIfFailed(hr, TEXT("Failed to resize DXGI swap chain."));

				m_pDXGISwapChain->GetDesc1(&DXGIDesc);
				m_Desc.AllowTearing = DXGIDesc.Flags & DXGI_PRESENT_ALLOW_TEARING;
			}

			void D3D12SwapChain::BindSwapChainBackBuffers()
			{
				// TODO: This doesnt belong here move it out to a higher level application context.
				//		ISwapchain is only supposed to describe a pure swapchain not rendering functionality.

				for (UInt32 i = 0; i < m_Desc.BufferCount; i++)
				{
					Microsoft::WRL::ComPtr<ID3D12Resource> DisplayPlane;
					ThrowIfFailed(m_pDXGISwapChain->GetBuffer(i, IID_PPV_ARGS(&DisplayPlane))
						, TEXT("Failed to bind buffer with DXGI swapchain back buffer!"));
					
					IE_ASSERT(m_DisplayPlanes[i] != NULL);
					m_DisplayPlanes[i]->CreateFromSwapChain(m_pDeviceRef, TEXT("SwapChain display plane"), DisplayPlane.Detach());
				}
			}
		}
	}
}
