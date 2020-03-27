#include "ie_pch.h"

#include "Direct3D12_Context.h"

using namespace Microsoft::WRL;

namespace Insight {
	


	Direct3D12Context::Direct3D12Context(HWND * windowHandle, uint32_t windowWidth, uint32_t windowHeight)
		: m_WindowHandle(windowHandle), RenderingContext(windowWidth, windowHeight)
	{
		IE_CORE_ASSERT(windowHandle, "Window handle is NULL!");
	}

	Direct3D12Context::~Direct3D12Context()
	{
	}

	void Direct3D12Context::OnUpdate()
	{
	}

	void Direct3D12Context::Init()
	{
		UINT dxgiFactoryFlags = 0;

#ifdef _DEBUG
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif // _DEBUG

		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_DxgiFactory)));

		if (m_UseWarpDevice)
		{
			ComPtr<IDXGIAdapter> warpAdapter;
			ThrowIfFailed(m_DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

			ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_Device)));
		}
		else
		{
			ComPtr<IDXGIAdapter1> hardwareAdapter;
			GetHardwareAdapter(m_DxgiFactory.Get(), &hardwareAdapter);

			ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_Device)));
		}
		
		CreateCommandQueue();
		CreateSwapChain();
		CreateRTVDescriptorHeap();
		CreateCommandAllocators();
		CreateFenceEvent();
		//CreateRootSignature();

		//TODO: Move shader creation to shader class
		//InitShaders();

	}

	void Direct3D12Context::WaitForPreviousFrame()
	{
		HRESULT hr;

		// swap the current rtv buffer index so we draw on the correct buffer
		m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

		// if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
		// the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
		if (m_Fence[m_FrameIndex]->GetCompletedValue() < m_FenceValue[m_FrameIndex])
		{
			// we have the fence create an event which is signaled once the fence's current value is "fenceValue"
			hr = m_Fence[m_FrameIndex]->SetEventOnCompletion(m_FenceValue[m_FrameIndex], m_FenceEvent);
			if (FAILED(hr))
			{
				throw std::exception();
			}

			// We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
			// has reached "fenceValue", we know the command queue has finished executing
			WaitForSingleObject(m_FenceEvent, INFINITE);
		}

		// increment fenceValue for next frame
		m_FenceValue[m_FrameIndex]++;
	}

	void Direct3D12Context::UpdatePipeline()
	{
		HRESULT hr;

		WaitForPreviousFrame();
		hr = m_CommandAllocators[m_FrameIndex]->Reset();
		if (FAILED(hr))
		{
			throw std::exception();
		}

		hr = m_CommandList->Reset(m_CommandAllocators[m_FrameIndex].Get(), NULL);
		if (FAILED(hr))
		{
			throw std::exception();
		}

		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);

		m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		const float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		hr = m_CommandList->Close();
		if (FAILED(hr))
		{
			throw std::exception();
		}

	}
	
	void Direct3D12Context::RenderFrame()
	{
		HRESULT hr;

		UpdatePipeline();

		ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };

		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		hr = m_CommandQueue->Signal(m_Fence[m_FrameIndex].Get(), m_FenceValue[m_FrameIndex]);

	}

	void Direct3D12Context::SwapBuffers()
	{
		HRESULT hr = m_SwapChain->Present(0, 0);
	}
	
	void Direct3D12Context::CreateSwapChain()
	{
		HRESULT hr;

		DXGI_MODE_DESC backBufferDesc = {};
		backBufferDesc.Width = m_WindowWidth;
		backBufferDesc.Height = m_WindowHeight;

		DXGI_SAMPLE_DESC sampleDesc = {};
		sampleDesc.Count = 1;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = m_FrameBufferCount;
		swapChainDesc.Width = m_WindowWidth;
		swapChainDesc.Height = m_WindowHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc = sampleDesc;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain{};
		hr = m_DxgiFactory->CreateSwapChainForHwnd(m_CommandQueue.Get(), *m_WindowHandle, &swapChainDesc, nullptr, nullptr, &swapChain);
		if (FAILED(hr)) {
			MessageBox(0, L"Failed to Create Swap Chain", L"Error", MB_OK);
		}
		hr = m_DxgiFactory->MakeWindowAssociation(*m_WindowHandle, DXGI_MWA_NO_ALT_ENTER);
		if (FAILED(hr)) {
			MessageBox(0, L"Failed to Make Window Association", L"Error", MB_OK);
		}
		hr = swapChain.As(&m_SwapChain);
		if (FAILED(hr)) {
			MessageBox(0, L"Failed to Cast ComPtr", L"Error", MB_OK);
		}
		m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	}

	void Direct3D12Context::CreateRTVDescriptorHeap()
	{
		HRESULT hr;
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = m_FrameBufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		hr = m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RtvDescriptorHeap));
		if (FAILED(hr))
		{
			MessageBox(0, L"Failed to Create Descriptor Heap", L"Error", MB_OK);
		}
		m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		for (int i = 0; i < m_FrameBufferCount; i++)
		{
			hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));
			if (FAILED(hr))
			{
				MessageBox(0, L"Failed to Initialize Render Targets", L"Error", MB_OK);
			}
			m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);

			rtvHandle.Offset(1, m_RtvDescriptorSize);
		}

	}

	void Direct3D12Context::CreateCommandAllocators()
	{
		HRESULT hr;
		for (int i = 0; i < m_FrameBufferCount; i++)
		{
			hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocators[i]));
			if (FAILED(hr))
			{
				MessageBox(0, L"Failed to Create Command Allocator", L"Error", MB_OK);
			}
		}

		hr = m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_CommandList));
		if (FAILED(hr))
		{
			MessageBox(0, L"Failed to Create Command List", L"Error", MB_OK);
		}

		m_CommandList->Close();

	}

	void Direct3D12Context::CreateFenceEvent()
	{
		HRESULT hr;
		for (int i = 0; i < m_FrameBufferCount; i++)
		{
			hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence[i]));
			if (FAILED(hr))
			{
				MessageBox(0, L"Failed to Create Fence", L"Error", MB_OK);
			}
			m_FenceValue[i] = 0;
		}

		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_FenceEvent == nullptr)
		{
			MessageBox(0, L"Fence Event was nullptr", L"Error", MB_OK);
		}
	}

	void Direct3D12Context::CreateRootSignature()
	{
		HRESULT hr;

		D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
		rootCBVDescriptor.RegisterSpace = 0;
		rootCBVDescriptor.ShaderRegister = 0;

		D3D12_DESCRIPTOR_RANGE descriptorTableRanges[1];
		descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; 
		descriptorTableRanges[0].NumDescriptors = 1; 
		descriptorTableRanges[0].BaseShaderRegister = 0; 
		descriptorTableRanges[0].RegisterSpace = 0; 
		descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; 

		D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
		descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges); 
		descriptorTable.pDescriptorRanges = &descriptorTableRanges[0];

		D3D12_ROOT_PARAMETER rootPerameters[2]; 
		rootPerameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; 
		rootPerameters[0].Descriptor = rootCBVDescriptor;
		rootPerameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
																			 
		rootPerameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootPerameters[1].DescriptorTable = descriptorTable; 
		rootPerameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 0;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(_countof(rootPerameters),// We have one root parameter
			rootPerameters, // A pointer to the beginning of our root parameters array
			1,
			&sampler,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // We can deny shader stages here for better performance
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
		);

		ID3DBlob* signature;
		hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
		if (FAILED(hr))
		{
			MessageBox(0, L"Failed to Serialize Root Signature", L"Error", MB_OK);
		}
		hr = m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		if (FAILED(hr))
		{
			MessageBox(0, L"Failed to create root signature", L"Error", MB_OK);
		}
	}

	void Direct3D12Context::InitShaders()
	{
		HRESULT hr;

		// Compile vertex shader
		ID3DBlob* vertexShader; // d3d blob for holding vertex shader bytecode
		ID3DBlob* errorBuffer; // A buffer holding the error data if any
		hr = D3DCompileFromFile(L"VertexShader.hlsl",
			nullptr,
			nullptr,
			"main",
			"vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&vertexShader,
			&errorBuffer);

		if (FAILED(hr))
		{
			OutputDebugStringA((char*)errorBuffer->GetBufferPointer());
			__debugbreak();
		}

		// Fill out a shader bytecode structure, which is basically just a pointer
		// to the shader bytecode and the size of the shader bytecode
		D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
		vertexShaderBytecode.BytecodeLength = vertexShader->GetBufferSize();
		vertexShaderBytecode.pShaderBytecode = vertexShader->GetBufferPointer();

		// Compile shader
		ID3DBlob* pixelShader;
		hr = D3DCompileFromFile(L"PixelShader.hlsl",
			nullptr,
			nullptr,
			"main",
			"ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&pixelShader,
			&errorBuffer);
		if (FAILED(hr))
		{
			OutputDebugStringA((char*)errorBuffer->GetBufferPointer());
			__debugbreak();
		}

		// Fill Out shader bytecode structure for pixel shader
		D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
		pixelShaderBytecode.BytecodeLength = pixelShader->GetBufferSize();
		pixelShaderBytecode.pShaderBytecode = pixelShader->GetBufferPointer();

		// Create Input layout

		// The input layout is used by the Input Assembler so that it knows
		// how to read the vertex data bound to it.

		D3D12_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		// Fill out an inpu layout description structure
		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};

		// We can get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"

		inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
		inputLayoutDesc.pInputElementDescs = inputLayout;

		//cb_vertexShader.Initialize(pDevice.Get(), pCommandList.Get());
	}

	void Direct3D12Context::CreateCommandQueue()
	{
		HRESULT hr;
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		hr = m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue));
		if (FAILED(hr)) {
			MessageBox(0, L"Failed to Create Command Queue", L"Error", MB_OK);

		}
	}

	void Direct3D12Context::GetHardwareAdapter(IDXGIFactory2 * pFactory, IDXGIAdapter1 ** ppAdapter)
	{
		ComPtr<IDXGIAdapter1> adapter;
		*ppAdapter = nullptr;

		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// We dont want software adapters
				continue;
			}

			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), m_RayTraceEnabled ? D3D_FEATURE_LEVEL_12_1 : D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
				break;
		}
		*ppAdapter = adapter.Detach();
	}

}