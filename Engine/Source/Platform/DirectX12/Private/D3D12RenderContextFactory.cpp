#include <Engine_pch.h>

#include "Platform/DirectX12/Public/D3D12RenderContextFactory.h"
#include "Platform/DirectX12/Public/D3D12RenderContext.h"
#include "Platform/DirectX12/Public/D3D12Device.h"
#include "Platform/DirectX12/Private/D3D12SwapChain.h"
#include "Platform/DirectX12/Private/D3D12CommandManager.h"
#include "Platform/Win32/Error/COMException.h"

#include "Runtime/Graphics/Public/IRenderContext.h"
#include "Runtime/Core/Window.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{

			D3D12RenderContextFactory::D3D12RenderContextFactory()
				: m_pTarget(NULL)
				, m_pDXGIFactory(NULL)
			{
				CreateDXGIFactory();
			}

			D3D12RenderContextFactory::~D3D12RenderContextFactory()
			{
				COM_SAFE_RELEASE(m_pDXGIFactory);
			}

			void D3D12RenderContextFactory::CreateContext(IRenderContext** OutContext, std::shared_ptr<Window> pWindow)
			{
				IE_ASSERT(m_pDXGIFactory != NULL);
				IE_ASSERT(OutContext != NULL);

				(*OutContext) = new D3D12RenderContext();
				m_pTarget = (*OutContext);
				m_pTarget->SetWindow(pWindow);
				
				CreateDevice(m_pTarget->GetDevice());
				CreateCommandManager(m_pTarget->GetCommandManager(), *m_pTarget->GetDevice());
				CreateSwapChain(m_pTarget->GetSwapChain(), *m_pTarget->GetCommandManager());
			}

			void D3D12RenderContextFactory::CreateDevice(IDevice** OutDevice)
			{
				(*OutDevice) = new D3D12Device();
				D3D12Device* pD3D12Device = DCast<D3D12Device*>((*OutDevice));
				IE_ASSERT(pD3D12Device != NULL);

				IED3D12DeviceInitParams DeviceInitParams;
				ZeroMem(&DeviceInitParams);
				DeviceInitParams.CheckForDXRSupport = true;
				DeviceInitParams.ForceWarpAdapter	= false;
				DeviceInitParams.MinFeatureLevel	= D3D_FEATURE_LEVEL_11_0;
				DeviceInitParams.MinDXRFeatureLevel = D3D_FEATURE_LEVEL_12_1;
				IED3D12DeviceQueryResult DeviceQueryResult;
				ZeroMem(&DeviceQueryResult);
				pD3D12Device->Initialize(DeviceInitParams, DeviceQueryResult, RCast<void**>(&m_pDXGIFactory));
			}

			void D3D12RenderContextFactory::CreateDXGIFactory()
			{
				UInt32 dxgiFactoryFlags = 0;

#			if IE_DEBUG
				// Enable the debug layer (requires the Graphics Tools "optional feature").
				// NOTE: Enabling the debug layer after device creation will invalidate the active device.
				{
					Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
					if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
					{
						debugController->EnableDebugLayer();

						// Enable additional debug layers.
						dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
					}

					Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
					if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
					{
						dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
						dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

						DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
						{
							80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
							D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,
						};
						DXGI_INFO_QUEUE_FILTER filter = {};
						filter.DenyList.NumIDs = _countof(hide);
						filter.DenyList.pIDList = hide;
						dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
					}
				}
#			endif

				ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_pDXGIFactory)), TEXT("Failed to create DXGI factory!"));
			}

			void D3D12RenderContextFactory::CreateSwapChain(ISwapChain** OutSwapChain, ICommandManager* InCommandManager)
			{
				(*OutSwapChain) = new D3D12SwapChain();
				D3D12SwapChain* pD3D12SwapChain = DCast<D3D12SwapChain*>((*OutSwapChain));
				IE_ASSERT(pD3D12SwapChain != NULL); // Trying to create a swap chain with an invalid device.

				D3D12CommandManager* pCommandManager = DCast<D3D12CommandManager*>(InCommandManager);
				IE_ASSERT(pCommandManager != NULL); // Trying to create swap chain with invalid command manager.

				D3D12CommandQueue* pD3D12CommandQueue = RCast<D3D12CommandQueue*>(pCommandManager->GetGraphicsQueue());
				IESwapChainCreateDesc SwapChainInitParams;
				ZeroMem(&SwapChainInitParams);
				SwapChainInitParams.Width = m_pTarget->GetWindow()->GetWidth();
				SwapChainInitParams.Height = m_pTarget->GetWindow()->GetHeight();
				SwapChainInitParams.BufferCount = 3;
				SwapChainInitParams.Format = TF_RGB8_UNORM;
				SwapChainInitParams.SampleDesc.Count = 1;
				SwapChainInitParams.SampleDesc.Quality = 0;
				SwapChainInitParams.NativeWindow = m_pTarget->GetWindow()->GetNativeWindow();
				SwapChainInitParams.AllowTearing = true;
				pD3D12SwapChain->Initialize(SwapChainInitParams, &m_pDXGIFactory, pD3D12CommandQueue);
			}

			void D3D12RenderContextFactory::CreateCommandManager(ICommandManager** OutCommandManager, IDevice* InDevice)
			{
				IE_ASSERT(InDevice != NULL); // Trying to create command manager with null device.
				(*OutCommandManager) = new D3D12CommandManager();
				D3D12CommandManager* pD3D12CommandManager = DCast<D3D12CommandManager*>((*OutCommandManager));
				IE_ASSERT(pD3D12CommandManager);

				pD3D12CommandManager->Initialize(InDevice);
			}
		}
	}
}
