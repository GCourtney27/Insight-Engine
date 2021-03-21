#include <Engine_pch.h>

#include "D3D12DeviceResources.h"

#include "Platform/DirectX12/Direct3D12Context.h"
#include "Platform/Win32/Win32Window.h"

namespace Insight {


	/*
		Feature levels the application will target.
	*/
	constexpr D3D_FEATURE_LEVEL cx_TargetFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	constexpr D3D_FEATURE_LEVEL cx_RayTraceTargetFeatureLevel = D3D_FEATURE_LEVEL_12_1;


	D3D12DeviceResources::D3D12DeviceResources()
		: m_ForceUseWarpAdapter(false)
	{
	}

	D3D12DeviceResources::~D3D12DeviceResources()
	{
	}

	bool D3D12DeviceResources::Init(Direct3D12Context* pRendererContext)
	{
		m_pRenderContextRef = pRendererContext;
		CreateDXGIFactory();
		CreateDevice();
		CreateCommandQueues();
		CreateSwapChain();
		CreateD3D11On12Resources();
		CreateSwapchainRTVDescriptors();

		CreateViewport();
		CreateScissorRect();


		CreateFenceEvent();

		return true;
	}

	void D3D12DeviceResources::ResizeResources()
	{
		uint32_t WindowWidth = m_pRenderContextRef->GetWindowRef().GetWidth();
		uint32_t WindowHeight = m_pRenderContextRef->GetWindowRef().GetHeight();

		m_d2dDevice.Reset();
		m_d2dFactory.Reset();
		m_dWriteFactory.Reset();
		m_D3D11On12Device.Reset();
		m_d2dDeviceContext.Reset();
		m_pD3D11DeviceContext.Reset(); 

		for (int i = 0; i < m_FrameBufferCount; ++i)
		{
			// Release the swapchain resources.
			m_pSwapChainRenderTargets[i].Reset();
			m_WrappedBackBuffers[i].Reset();
			m_d2dRenderTargets[i].Reset();

			// Reset the fence values to sync future frames.
			ResetFenceValue(i);
		}

		// Resize the swapchain
		{
			DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
			m_pSwapChain->GetDesc(&SwapChainDesc);
			HRESULT hr = m_pSwapChain->ResizeBuffers(SwapChainDesc.BufferCount, WindowWidth, WindowHeight, SwapChainDesc.BufferDesc.Format, SwapChainDesc.Flags);
			ThrowIfFailed(hr, TEXT("Failed to resize swap chain buffers for D3D 12 context."));

			CreateD3D11On12Resources();
			CreateSwapchainRTVDescriptors();
		}

		// Update View and Scissor rects
		{
			m_Client_ViewPort.TopLeftX = 0.0f;
			m_Client_ViewPort.TopLeftY = 0.0f;
			m_Client_ViewPort.Width = static_cast<FLOAT>(WindowWidth);
			m_Client_ViewPort.Height = static_cast<FLOAT>(WindowHeight);

			m_Client_ScissorRect.left = static_cast<LONG>(m_Client_ViewPort.TopLeftX);
			m_Client_ScissorRect.right = static_cast<LONG>(m_Client_ViewPort.TopLeftX + m_Client_ViewPort.Width);
			m_Client_ScissorRect.top = static_cast<LONG>(m_Client_ViewPort.TopLeftY);
			m_Client_ScissorRect.bottom = static_cast<LONG>(m_Client_ViewPort.TopLeftX + m_Client_ViewPort.Height);
		}
	}

	void D3D12DeviceResources::CleanUp()
	{
		// Close the fence handle on the GPU
		if (!CloseHandle(m_FenceEvent)) {
			IE_LOG(Error, TEXT("Failed to close GPU handle while cleaning up the D3D 12 context."));
		}
	}

	void D3D12DeviceResources::CreateD3D11On12Resources()
	{
		UINT D3D11DeviceCreateFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};

#if defined (IE_DEBUG)
		D3D11DeviceCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;

		// Filter a debug error coming from the 11on12 layer.
		Microsoft::WRL::ComPtr<ID3D12InfoQueue> InfoQueue;
		if (SUCCEEDED(m_pD3D12Device->QueryInterface(IID_PPV_ARGS(&InfoQueue))))
		{
			// Suppress whole categories of messages.
			//D3D12_MESSAGE_CATEGORY categories[] = {};

			// Suppress messages based on their severity level.
			D3D12_MESSAGE_SEVERITY Severities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO,
			};

			// Suppress individual messages by their ID.
			D3D12_MESSAGE_ID DenyIds[] =
			{
				// This occurs when there are uninitialized descriptors in a descriptor table, even when a
				// shader does not access the missing descriptors.
				D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,
			};

			D3D12_INFO_QUEUE_FILTER Filter = {};
			//filter.DenyList.NumCategories = _countof(categories);
			//filter.DenyList.pCategoryList = categories;
			Filter.DenyList.NumSeverities = _countof(Severities);
			Filter.DenyList.pSeverityList = Severities;
			Filter.DenyList.NumIDs = _countof(DenyIds);
			Filter.DenyList.pIDList = DenyIds;

			ThrowIfFailed(InfoQueue->PushStorageFilter(&Filter), TEXT("Failed to push debug storage filter."));
		}
#endif

		// Create the virtual device.
		Microsoft::WRL::ComPtr<ID3D11Device> D3D11Device;
		HRESULT hr = D3D11On12CreateDevice(
			m_pD3D12Device.Get(),	// D3D12 Device
			D3D11DeviceCreateFlags,	// Device Create Flags
			nullptr,				// Feature Levels
			0,						// Num Feature Levels
			reinterpret_cast<::IUnknown**>(m_pGraphicsCommandQueue.GetAddressOf()), // Command Queue
			1,						// Num Queues
			0,						// Node Mask
			D3D11Device.GetAddressOf(),				// D3D11 Device
			m_pD3D11DeviceContext.GetAddressOf(),	// D3D11 Device Context
			nullptr
		);
		ThrowIfFailed(hr, TEXT("Failed to create D3D11On12 device."));

		hr = D3D11Device.As(&m_D3D11On12Device);
		ThrowIfFailed(hr, TEXT("Failed to cast D3D11 device as D3D11On12."));

		// Create D2D/DWrite componenets
		D2D1_DEVICE_CONTEXT_OPTIONS DeviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, &m_d2dFactory);
		ThrowIfFailed(hr, TEXT(""));
		Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
		ThrowIfFailed(m_D3D11On12Device.As(&dxgiDevice), TEXT(""));
		ThrowIfFailed(m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice), TEXT(""));
		ThrowIfFailed(m_d2dDevice->CreateDeviceContext(DeviceOptions, &m_d2dDeviceContext), TEXT(""));
		ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &m_dWriteFactory), TEXT(""));

	}

	void D3D12DeviceResources::CreateDXGIFactory()
	{
		UINT DxgiFactoryFlags = 0u;

		// Enable debug layers if in debug builds
#if defined (IE_DEBUG)
		{
			Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();
				DxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
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
#endif

		HRESULT hr = CreateDXGIFactory2(DxgiFactoryFlags, IID_PPV_ARGS(&m_pDxgiFactory));
		ThrowIfFailed(hr, TEXT("Failed to create DXGI Factory."));
	}

	void D3D12DeviceResources::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter;
		*ppAdapter = nullptr;
		UINT CurrentMemory = 0;
		DXGI_ADAPTER_DESC1 Desc;

		

		auto CheckSM6_0Support = [](ID3D12Device* pDevice)
		{
			IE_ASSERT(pDevice);
			D3D12_FEATURE_DATA_SHADER_MODEL sm6_0{ D3D_SHADER_MODEL_6_0 };
			HRESULT hr = pDevice->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &sm6_0, sizeof(sm6_0));
			ThrowIfFailed(hr, TEXT("Failed to query feature support for shader model 6 with device."));
			if (sm6_0.HighestShaderModel >= D3D_SHADER_MODEL_6_0)
				return true;
			else
				return false;
		};

		auto CheckRayTracingSupport = [](ID3D12Device* pDevice)
		{
			IE_ASSERT(pDevice);
			D3D12_FEATURE_DATA_D3D12_OPTIONS5 Options5 = {};
			HRESULT hr = pDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &Options5, sizeof(Options5));
			ThrowIfFailed(hr, TEXT("Failed to query feature support for ray trace with device."));
			if (Options5.RaytracingTier < D3D12_RAYTRACING_TIER_1_0)
				return false;
			else
				return true;
		};

		for (UINT AdapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(AdapterIndex, &pAdapter); ++AdapterIndex)
		{
			Desc = {};
			pAdapter->GetDesc1(&Desc);

			// Make sure we get the video card that is not a software adapter
			// and it has the most video memory.
			if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE || Desc.DedicatedVideoMemory < CurrentMemory) continue;

			// Check if we can support ray tracing with the device.
			if (m_pRenderContextRef->GetIsRayTraceEnabled())
			{
				// Pass in a temp device to poll feature support later.
				Microsoft::WRL::ComPtr<ID3D12Device5> TempDevice;
				if (SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), cx_TargetFeatureLevel, __uuidof(ID3D12Device5), &TempDevice)))
				{
					// If the device supports the feature level check to see if it can support DXR.
					if (CheckRayTracingSupport(TempDevice.Get()))
					{
						CurrentMemory = static_cast<UINT>(Desc.DedicatedVideoMemory);
						if (*ppAdapter != nullptr)
							(*ppAdapter)->Release();

						*ppAdapter = pAdapter.Detach();
						m_pRenderContextRef->SetIsRayTraceSupported(true);

						IE_LOG(Log, TEXT("Found suitable D3D 12 hardware that can support DXR: %s"), Desc.Description);
						continue;
					}
				}
			}

			// If we cannot support ray tracing, just see if D3D 12 is supported and create a default device.
			if (SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), cx_TargetFeatureLevel, __uuidof(ID3D12Device), nullptr)))
			{
				CurrentMemory = static_cast<UINT>(Desc.DedicatedVideoMemory);
				if (*ppAdapter != nullptr) (*ppAdapter)->Release();

				*ppAdapter = pAdapter.Detach();
				IE_LOG(Log, TEXT("Found suitable D3D 12 hardware: %s"), Desc.Description);
			}


		}
		Desc = {};
		(*ppAdapter)->GetDesc1(&Desc);
		IE_LOG(Warning, TEXT("\"%s\" selected as D3D 12 graphics hardware."), Desc.Description);
	}

	void D3D12DeviceResources::CreateDevice()
	{
		if (m_ForceUseWarpAdapter)
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
			HRESULT hr = m_pDxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
			ThrowIfFailed(hr, TEXT("Failed to enumerate WARP adapter for D3D12."));

			hr = D3D12CreateDevice(warpAdapter.Get(), cx_TargetFeatureLevel, IID_PPV_ARGS(&m_pD3D12Device));
			ThrowIfFailed(hr, TEXT("Failed to create WARP device for D3D12."));

			// WARP adapters are software adapters. They almost never support DXR Ray Tracing.
			m_pRenderContextRef->SetIsRayTraceSupported(false);

			DXGI_ADAPTER_DESC Desc;
			warpAdapter->GetDesc(&Desc);
			IE_LOG(Warning, TEXT("\"%s\" selected as D3D 12 graphics hardware."), StringHelper::WideToString(Desc.Description).c_str());
		}
		else
		{
			GetHardwareAdapter(m_pDxgiFactory.Get(), &m_pAdapter);

			// If ray tracing is supported create the device DXR needs
			if (m_pRenderContextRef->m_IsRayTraceSupported)
			{
				HRESULT hr = D3D12CreateDevice(m_pAdapter.Get(), cx_RayTraceTargetFeatureLevel, IID_PPV_ARGS(&m_pD3D12Device));
				ThrowIfFailed(hr, TEXT("Failed to create D3D12 device for ray tracing."));
			}
			else
			{
				HRESULT hr = D3D12CreateDevice(m_pAdapter.Get(), cx_TargetFeatureLevel, IID_PPV_ARGS(&m_pD3D12Device));
				ThrowIfFailed(hr, TEXT("Failed to create D3D 12 device."));
			}

			D3D_FEATURE_LEVEL FeatureLevels[] = {
				D3D_FEATURE_LEVEL_12_1,
				D3D_FEATURE_LEVEL_12_0,
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_3,
				D3D_FEATURE_LEVEL_9_2,
				D3D_FEATURE_LEVEL_9_1
			};
			int NumLevels = _countof(FeatureLevels);

			D3D12_FEATURE_DATA_FEATURE_LEVELS FormatInfo = {  };
			FormatInfo.NumFeatureLevels = NumLevels;
			FormatInfo.pFeatureLevelsRequested = FeatureLevels;
			HRESULT hr = m_pD3D12Device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &FormatInfo, sizeof(FormatInfo));
			IE_LOG(Log, TEXT(""));
		}
	}

	void D3D12DeviceResources::CreateCommandQueues()
	{
		HRESULT hr;

		D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
		QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		// Create Graphics Command Queue
		{
			QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

			hr = m_pD3D12Device->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&m_pGraphicsCommandQueue));
			ThrowIfFailed(hr, TEXT("Failed to create graphics command queue."));
		}

		// Create Compute Command Queue
		{
			QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

			hr = m_pD3D12Device->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&m_pComputeCommandQueue));
			ThrowIfFailed(hr, TEXT("Failed to create compute command queue."));
		}
	}

	void D3D12DeviceResources::CreateSwapChain()
	{
		HRESULT hr;

		uint32_t WindowWidth = m_pRenderContextRef->GetWindowRef().GetWidth();
		uint32_t WindowHeight = m_pRenderContextRef->GetWindowRef().GetHeight();

		m_SampleDesc = {};
		m_SampleDesc.Count = 1;

		DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
		SwapChainDesc.BufferCount = m_FrameBufferCount;
		SwapChainDesc.Width = WindowWidth;
		SwapChainDesc.Height = WindowHeight;
		SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // Dont save the contents of the back buffer after presented
		SwapChainDesc.Flags = m_pRenderContextRef->m_AllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
		SwapChainDesc.SampleDesc = m_SampleDesc;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> TempSwapChain = {};
#if IE_PLATFORM_BUILD_WIN32
		hr = m_pDxgiFactory->CreateSwapChainForHwnd(
			m_pGraphicsCommandQueue.Get(),
			static_cast<HWND>(m_pRenderContextRef->GetWindowRef().GetNativeWindow()),
			&SwapChainDesc,
			nullptr,
			nullptr,
			&TempSwapChain
		);
		ThrowIfFailed(hr, TEXT("Failed to Create Swap Chain"));
		if (m_pRenderContextRef->m_AllowTearing)
		{
			ThrowIfFailed(m_pDxgiFactory->MakeWindowAssociation(m_pRenderContextRef->GetWindowRefAs<Win32Window>().GetWindowHandleRef(), DXGI_MWA_NO_ALT_ENTER),
				TEXT("Failed to Make Window Association"));
		}
#elif IE_PLATFORM_BUILD_UWP
		hr = m_pDxgiFactory->CreateSwapChainForCoreWindow(
			m_pGraphicsCommandQueue.Get(),
			reinterpret_cast<::IUnknown*>(m_pRenderContextRef->GetWindowRef().GetNativeWindow()),
			&SwapChainDesc,
			nullptr,
			&TempSwapChain
		);
		ThrowIfFailed(hr, TEXT("Failed to Create Swap Chain for UWP CoreWindow."));
#endif

		ThrowIfFailed(TempSwapChain.As(&m_pSwapChain), TEXT("Failed to cast SwapChain ComPtr"));
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		DXGI_SWAP_CHAIN_DESC1 Desc = {};
		m_pSwapChain->GetDesc1(&Desc);
		m_SwapChainBackBufferFormat = Desc.Format;


	}

	void D3D12DeviceResources::CreateSwapchainRTVDescriptors()
	{
		HRESULT hr;

		m_SwapChainRTVHeap.pDH.Reset();
		m_SwapChainRTVHeap.Create(m_pD3D12Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_FrameBufferCount, false);



		float DPI = m_pRenderContextRef->GetWindowRef().GetDPI();
		//float DPIX, DPIY;
		//m_d2dFactory->GetDesktopDpi(&DPIX, &DPIY);
		D2D1_BITMAP_PROPERTIES1 BitmapProps = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			DPI,
			DPI
		);

		for (UINT i = 0; i < m_FrameBufferCount; i++)
		{
			hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pSwapChainRenderTargets[i]));
			ThrowIfFailed(hr, TEXT("Failed to get buffer in swap chain during descriptor heap initialization for D3D 12 context."));
			m_pD3D12Device->CreateRenderTargetView(m_pSwapChainRenderTargets[i].Get(), nullptr, m_SwapChainRTVHeap.hCPU(i));

			WCHAR name[32];
			swprintf_s(name, L"SwapChain Render Target %d", i);
			m_pSwapChainRenderTargets[i]->SetName(name);

			// Create a wrapped 11On12 resource of this back buffer. Since we are 
			// rendering all D3D12 content first and then all D2D content, we specify 
			// the In resource state as RENDER_TARGET - because D3D12 will have last 
			// used it in this state - and the Out resource state as PRESENT. When 
			// ReleaseWrappedResources() is called on the 11On12 device, the resource 
			// will be transitioned to the PRESENT state.
			D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
			m_D3D11On12Device->CreateWrappedResource(
				m_pSwapChainRenderTargets[i].Get(),
				&d3d11Flags,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT,
				IID_PPV_ARGS(&m_WrappedBackBuffers[i])
			);

			// Create a render target for D2D to draw directly to this back buffer.
			Microsoft::WRL::ComPtr<IDXGISurface> pSurface;
			ThrowIfFailed(m_WrappedBackBuffers[i].As(&pSurface), TEXT("Failed to cast surface as wrapped buffer."));
			m_d2dDeviceContext->CreateBitmapFromDxgiSurface(pSurface.Get(), &BitmapProps, &m_d2dRenderTargets[i]);
		}
	}

	void D3D12DeviceResources::CreateViewport()
	{
		uint32_t WindowWidth = m_pRenderContextRef->GetWindowRef().GetWidth();
		uint32_t WindowHeight = m_pRenderContextRef->GetWindowRef().GetHeight();

		m_Client_ViewPort.TopLeftX = 0;
		m_Client_ViewPort.TopLeftY = 0;
		m_Client_ViewPort.Width = static_cast<FLOAT>(WindowWidth);
		m_Client_ViewPort.Height = static_cast<FLOAT>(WindowHeight);
		m_Client_ViewPort.MinDepth = 0.0f;
		m_Client_ViewPort.MaxDepth = 1.0f;
	}

	void D3D12DeviceResources::CreateScissorRect()
	{
		uint32_t WindowWidth = m_pRenderContextRef->GetWindowRef().GetWidth();
		uint32_t WindowHeight = m_pRenderContextRef->GetWindowRef().GetHeight();

		m_Client_ScissorRect.left = 0;
		m_Client_ScissorRect.top = 0;
		m_Client_ScissorRect.right = static_cast<LONG>(WindowWidth);
		m_Client_ScissorRect.bottom = static_cast<LONG>(WindowHeight);
	}

	void D3D12DeviceResources::CreateFenceEvent()
	{
		HRESULT hr;
		hr = m_pD3D12Device->CreateFence(m_FenceValues[m_FrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
		ThrowIfFailed(hr, TEXT("Failed to create Fence Event"));
		m_FenceValues[m_FrameIndex]++;

		m_FenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		if (m_FenceEvent == nullptr) THROW_COM_ERROR(TEXT("Fence Event was nullptr"));

		m_ComputeFenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		if (m_ComputeFenceEvent == nullptr) THROW_COM_ERROR(TEXT("Fence Event was nullptr"));
	}

	void D3D12DeviceResources::MoveToNextFrame()
	{
		HRESULT hr;

		// Schedule a Signal command in the queues.
		const UINT64 currentFenceValue = m_FenceValues[m_FrameIndex];
		hr = m_pGraphicsCommandQueue->Signal(m_pFence.Get(), currentFenceValue);
		ThrowIfFailed(hr, TEXT("Failed to signal fence on graphics command queue"));

		hr = m_pComputeCommandQueue->Signal(m_pFence.Get(), currentFenceValue);
		ThrowIfFailed(hr, TEXT("Failed to signal fence on compute command queue"));

		// Advance the frame index.
		m_FrameIndex = (m_FrameIndex + 1) % m_FrameBufferCount;

		// Check to see if the next frame is ready to start.
		if (m_pFence->GetCompletedValue() < m_FenceValues[m_FrameIndex])
		{
			// Wait for the submitted work on the graphics queue to finish.
			hr = m_pFence->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_FenceEvent);
			ThrowIfFailed(hr, TEXT("Failed to set completion event on fence"));
			WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

			// Wait for the submitted work on the compute queue to finish.
			hr = m_pFence->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_ComputeFenceEvent);
			ThrowIfFailed(hr, TEXT("Failed to set completion event on fence"));
			WaitForSingleObjectEx(m_ComputeFenceEvent, INFINITE, FALSE);
		}

		// Set the fence value for the next frame.
		m_FenceValues[m_FrameIndex] = currentFenceValue + 1;
	}

	void D3D12DeviceResources::WaitForGPU()
	{
		// Schedule a Signal command in the queue.
		ThrowIfFailed(m_pGraphicsCommandQueue->Signal(m_pFence.Get(), m_FenceValues[m_FrameIndex]), TEXT("Fialed to signal fence event on graphics command queue."));

		// Wait until the fence has been processed.
		ThrowIfFailed(m_pFence->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_FenceEvent), TEXT("Failed to set completion event on fence."));
		WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

		// Schedule a Signal command in the queue.
		ThrowIfFailed(m_pComputeCommandQueue->Signal(m_pFence.Get(), m_FenceValues[m_FrameIndex]), TEXT("Fialed to signal fence event on graphics command queue."));

		// Wait until the fence has been processed.
		ThrowIfFailed(m_pFence->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_ComputeFenceEvent), TEXT("Failed to set completion event on fence."));
		WaitForSingleObjectEx(m_ComputeFenceEvent, INFINITE, FALSE);

		// Increment the fence value for the current frame.
		m_FenceValues[m_FrameIndex]++;
	}

}