#include <ie_pch.h>

#include "D3D12_Helper.h"

namespace Insight {



	D3D12Helper::D3D12Helper()
	{
		
	}

	D3D12Helper::~D3D12Helper()
	{
	}

	bool D3D12Helper::Init(UINT windowWidth, UINT windowHeight)
	{
		m_WindowWidth = windowWidth;
		m_WindowHeight = windowHeight;
		CreateDXGIFactory();
		CreateDevice();

		CreateCommandQueue();
		CreateCommandAllocators();

		return true;
	}

	void D3D12Helper::CreateDXGIFactory()
	{
		UINT dxgiFactoryFlags = 0u;

		// Enable debug layers if in debug builds
#if defined IE_DEBUG
		{
			ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();
				dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
		}
#endif

		HRESULT hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_pDxgiFactory));
		ThrowIfFailed(hr, "Failed to create DXGI Factory.");
	}

	void D3D12Helper::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
	{
		ComPtr<IDXGIAdapter1> adapter;
		*ppAdapter = nullptr;
		UINT currentVideoCardMemory = 0;
		DXGI_ADAPTER_DESC1 desc;

		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
		{
			desc = {};
			adapter->GetDesc1(&desc);

			// Make sure we get the video card that is not a software adapter
			// and it has the most video memory
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE || desc.DedicatedVideoMemory < currentVideoCardMemory)
				continue;

			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
			{
				currentVideoCardMemory = static_cast<UINT>(desc.DedicatedVideoMemory);
				if (*ppAdapter != nullptr)
					(*ppAdapter)->Release();
				*ppAdapter = adapter.Detach();
			}
		}
	}

	void D3D12Helper::CreateDevice()
	{
		GetHardwareAdapter(m_pDxgiFactory.Get(), &m_pPhysicalDevice);

		HRESULT hr = D3D12CreateDevice(m_pPhysicalDevice.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pLogicalDevice));
		ThrowIfFailed(hr, "Failed to create logical device.");
	}

	void D3D12Helper::CreateCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		HRESULT hr = m_pLogicalDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCommandQueue));
		ThrowIfFailed(hr, "Failed to Create Command Queue");
	}

	void D3D12Helper::CreateSwapChain()
	{
		HRESULT hr;

		DXGI_MODE_DESC backBufferDesc = {};
		backBufferDesc.Width = m_WindowWidth;
		backBufferDesc.Height = m_WindowHeight;

		//m_SampleDesc = {};
		//m_SampleDesc.Count = 1;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = m_FrameBufferCount;
		swapChainDesc.Width = m_WindowWidth;
		swapChainDesc.Height = m_WindowHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		//swapChainDesc.SampleDesc = m_SampleDesc;
		//swapChainDesc.Flags = m_AllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain{};

		//hr = m_pDxgiFactory->CreateSwapChainForHwnd(m_pCommandQueue.Get(), *m_pWindowHandle, &swapChainDesc, nullptr, nullptr, &swapChain);
		//ThrowIfFailed(hr, "Failed to Create Swap Chain");

		/*if (m_AllowTearing)
		{
			hr = m_pDxgiFactory->MakeWindowAssociation(*m_pWindowHandle, DXGI_MWA_NO_ALT_ENTER);
			ThrowIfFailed(hr, "Failed to Make Window Association");
		}*/

		hr = swapChain.As(&m_pSwapChain);
		ThrowIfFailed(hr, "Failed to cast SwapChain ComPtr");

		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	}

	void D3D12Helper::CreateViewport()
	{
	}

	void D3D12Helper::CreateScissorRect()
	{
	}

	void D3D12Helper::CreateCommandAllocators()
	{
		HRESULT hr;
		for (int i = 0; i < m_FrameBufferCount; i++)
		{
			hr = m_pLogicalDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocators[i]));
			ThrowIfFailed(hr, "Failed to Create Command Allocator");
		}

		hr = m_pLogicalDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pCommandList));
		ThrowIfFailed(hr, "Failed to Create Command List");
	}

	void D3D12Helper::CreateFenceEvent()
	{
		HRESULT hr;
		for (int i = 0; i < m_FrameBufferCount; i++)
		{
			hr = m_pLogicalDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFences[i]));
			ThrowIfFailed(hr, "Failed to create Fence on index" + std::to_string(i));

			m_FenceValues[i] = 0;
		}

		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_FenceEvent == nullptr)
			THROW_COM_ERROR("Fence Event was nullptr");
	}

}