#include <Engine_pch.h>

#include <Runtime/Core.h>
#include <Runtime/Graphics/Public/GraphicsCore.h>
#include "Platform/DirectX12/Public/D3D12RenderContextFactory.h"

#include "Runtime/Core/Window.h"

#include "Platform/DirectX12/Public/D3D12RenderContext.h"
#include "Platform/DirectX12/Public/D3D12CommandContext.h"
#include "Platform/DirectX12/Public/D3D12Device.h"
#include "Platform/DirectX12/Private/D3D12CommandManager.h"
#include "Platform/DirectX12/Private/D3D12SwapChain.h"
#include "Platform/DirectX12/Public/D3D12DescriptorAllocator.h"
#include "Platform/DirectX12/Public/D3D12GeometryBufferManager.h"
#include "Platform/DirectX12/Public/ResourceManagement/D3D12ConstantBufferManager.h"
#include "Platform/DirectX12/Public/ResourceManagement/D3D12TextureManager.h"
#include "Platform/Public/Utility/COMException.h"


namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{

			static D3D12Device s_D3D12Device;
			static D3D12SwapChain s_D3D12SwapChain;
			static D3D12CommandManager s_D3D12CmdManager;
			static D3D12ContextManager s_D3D12CtxManager;
			static D3D12GeometryBufferManager s_D3D12GeomBufferManager;
			static D3D12ConstantBufferManager s_D3D12ConstBuffManager;
			static D3D12TextureManager s_D3D12TexManager;
			static D3D12RenderContext s_D3D12Context;

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
				IE_ASSERT(m_pDXGIFactory != NULL); // Cannot create D3D12 context with null dxgi factory.
				IE_ASSERT(OutContext != NULL); // Cannot create render context will null target.

				(*OutContext) = &s_D3D12Context;
				Super::m_pTarget = (*OutContext);
				Super::m_pTarget->SetWindow(pWindow);

				// Initialize all main components for the renderer.
				Super::InitializeMainComponents();

				// Initialize heaps and other resources.
				IE_ASSERT(g_pDevice != NULL);
				g_pDevice->CreateDescriptorHeap(TEXT("Scene Texture Descriptors"), RHT_CBV_SRV_UAV, 4096, &g_pTextureHeap);
			}

			void D3D12RenderContextFactory::CreateDevice(IDevice** OutDevice)
			{
				(*OutDevice) = &s_D3D12Device;

				IED3D12DeviceInitParams DeviceInitParams;
				ZeroMem(&DeviceInitParams);
				DeviceInitParams.CheckForDXRSupport = true;
				DeviceInitParams.ForceWarpAdapter	= false;
				DeviceInitParams.MinFeatureLevel	= D3D_FEATURE_LEVEL_11_0;
				DeviceInitParams.MinDXRFeatureLevel = D3D_FEATURE_LEVEL_12_1;
				IED3D12DeviceQueryResult DeviceQueryResult;
				ZeroMem(&DeviceQueryResult);
				s_D3D12Device.Initialize(DeviceInitParams, DeviceQueryResult, RCast<void**>(&m_pDXGIFactory));
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
				(*OutSwapChain) = &s_D3D12SwapChain;
				
				D3D12CommandManager* pCommandManager = DCast<D3D12CommandManager*>(InCommandManager);
				IE_ASSERT(pCommandManager != NULL); // Trying to create swap chain with invalid command manager.

				IE_ASSERT(InDevice != NULL);
				ID3D12Device* pID3D12Device = RCast<ID3D12Device*>(InDevice->GetNativeDevice());

				D3D12CommandQueue* pD3D12CommandQueue = RCast<D3D12CommandQueue*>(pCommandManager->GetGraphicsQueue());
				IESwapChainDescription SwapChainInitParams;
				ZeroMem(&SwapChainInitParams);
				SwapChainInitParams.Width = Super::m_pTarget->GetWindow()->GetWidth();
				SwapChainInitParams.Height = Super::m_pTarget->GetWindow()->GetHeight();
				SwapChainInitParams.BufferCount = 3;
				SwapChainInitParams.Format = F_R8G8B8A8_UNorm;
				SwapChainInitParams.SampleDesc.Count = 1;
				SwapChainInitParams.SampleDesc.Quality = 0;
				SwapChainInitParams.NativeWindow = Super::m_pTarget->GetWindow()->GetNativeWindow();
				s_D3D12SwapChain.Initialize(InDevice);
				s_D3D12SwapChain.Create(SwapChainInitParams, &m_pDXGIFactory, pD3D12CommandQueue, pID3D12Device);
			}

			void D3D12RenderContextFactory::CreateCommandManager(ICommandManager** OutCommandManager, IDevice* InDevice)
			{
				IE_ASSERT(InDevice != NULL); // Trying to create command manager with null device.

				(*OutCommandManager) = &s_D3D12CmdManager;

				s_D3D12CmdManager.Initialize(InDevice);
			}
			
			void D3D12RenderContextFactory::CreateContextManager(IContextManager** OutCommandContext)
			{
				(*OutCommandContext) = &s_D3D12CtxManager;
			}
			
			void D3D12RenderContextFactory::CreateGeometryManager(IGeometryBufferManager** OutGeometryManager)
			{
				(*OutGeometryManager) = &s_D3D12GeomBufferManager;
			}
			
			void D3D12RenderContextFactory::CreateConstantBufferManager(IConstantBufferManager** OutCBManager)
			{
				(*OutCBManager) = &s_D3D12ConstBuffManager;

				s_D3D12ConstBuffManager.Initialize();
			}
			
			void D3D12RenderContextFactory::CreateTextureManager(ITextureManager** OutTexManager)
			{
				(*OutTexManager) = &s_D3D12TexManager;

				s_D3D12TexManager.Initialize();
			}
		}
	}
}
