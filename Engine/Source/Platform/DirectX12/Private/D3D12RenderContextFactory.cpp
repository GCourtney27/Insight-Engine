#include <Engine_pch.h>

#include "Platform/DirectX12/Public/D3D12RenderContextFactory.h"
#include "Platform/DirectX12/Public/D3D12RenderContext.h"
#include "Platform/DirectX12/Public/D3D12CommandContext.h"
#include "Platform/DirectX12/Public/D3D12Device.h"
#include "Platform/DirectX12/Private/D3D12CommandManager.h"
#include "Platform/DirectX12/Private/D3D12SwapChain.h"
#include "Platform/DirectX12/Public/D3D12DescriptorAllocator.h"
#include "Platform/DirectX12/Public/D3D12GeometryBufferManager.h"
#include "Platform/DirectX12/Public/ResourceManagement/D3D12ConstantBufferManager.h"
#include "Platform/Public/Utility/COMException.h"

#include "Runtime/Graphics/Public/IRenderContext.h"
#include "Runtime/Core/Window.h"

#include <Runtime/Graphics/Public/GraphicsCore.h>

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{

			D3D12RenderContextFactory::D3D12RenderContextFactory()
				: m_pDXGIFactory(NULL)
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
				Super::m_pTarget = (*OutContext);
				Super::m_pTarget->SetWindow(pWindow);

				Super::InitializeMainComponents();
			}

			void D3D12RenderContextFactory::CreateDevice(IDevice** OutDevice)
			{
				D3D12Device* pD3D12Device = CreateRenderComponentObject<D3D12Device>(OutDevice);

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

			void D3D12RenderContextFactory::CreateSwapChain(ISwapChain** OutSwapChain, ICommandManager* InCommandManager, IDevice* InDevice)
			{
				D3D12SwapChain* pD3D12SwapChain = CreateRenderComponentObject<D3D12SwapChain>(OutSwapChain);

				D3D12CommandManager* pCommandManager = DCast<D3D12CommandManager*>(InCommandManager);
				IE_ASSERT(pCommandManager != NULL); // Trying to create swap chain with invalid command manager.

				IE_ASSERT(InDevice != NULL);
				ID3D12Device* pID3D12Device = RCast<ID3D12Device*>(InDevice->GetNativeDevice());

				D3D12CommandQueue* pD3D12CommandQueue = RCast<D3D12CommandQueue*>(pCommandManager->GetGraphicsQueue());
				IESwapChainDesc SwapChainInitParams;
				ZeroMem(&SwapChainInitParams);
				SwapChainInitParams.Width = Super::m_pTarget->GetWindow()->GetWidth();
				SwapChainInitParams.Height = Super::m_pTarget->GetWindow()->GetHeight();
				SwapChainInitParams.BufferCount = 3;
				SwapChainInitParams.Format = F_R8G8B8A8_UNorm;
				SwapChainInitParams.SampleDesc.Count = 1;
				SwapChainInitParams.SampleDesc.Quality = 0;
				SwapChainInitParams.NativeWindow = Super::m_pTarget->GetWindow()->GetNativeWindow();
				pD3D12SwapChain->Initialize(InDevice);
				pD3D12SwapChain->Create(SwapChainInitParams, &m_pDXGIFactory, pD3D12CommandQueue, pID3D12Device);
			}

			void D3D12RenderContextFactory::CreateCommandManager(ICommandManager** OutCommandManager, IDevice* InDevice)
			{
				IE_ASSERT(InDevice != NULL); // Trying to create command manager with null device.

				D3D12CommandManager* pD3D12CommandManager = CreateRenderComponentObject<D3D12CommandManager>(OutCommandManager);
				IE_ASSERT(pD3D12CommandManager != NULL);

				pD3D12CommandManager->Initialize(InDevice);
			}
			
			void D3D12RenderContextFactory::CreateContextManager(IContextManager** OutCommandContext)
			{
				D3D12ContextManager* pD3D12ContextManager = CreateRenderComponentObject<D3D12ContextManager>(OutCommandContext);
				IE_ASSERT(pD3D12ContextManager != NULL);
			}
			
			void D3D12RenderContextFactory::CreateGeometryManager(IGeometryBufferManager** OutGeometryManager)
			{
				D3D12GeometryBufferManager* pD3D12ContextManager = CreateRenderComponentObject<D3D12GeometryBufferManager>(OutGeometryManager);
				IE_ASSERT(pD3D12ContextManager != NULL);
			}
			
			void D3D12RenderContextFactory::CreateConstantBufferManager(IConstantBufferManager** OutCBManager)
			{
				D3D12ConstantBufferManager* pD3D12CBManager = CreateRenderComponentObject<D3D12ConstantBufferManager>(OutCBManager);
				IE_ASSERT(pD3D12CBManager != NULL)
			}
		}
	}
}
