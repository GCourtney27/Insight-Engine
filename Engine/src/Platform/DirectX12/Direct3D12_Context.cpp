#include "ie_pch.h"

#include "Direct3D12_Context.h"
#include "Platform/Windows/Windows_Window.h"

#include "imgui.h"
#include "Platform/ImGui/ImGui_DX12_Renderer.h"
#include "Platform/ImGui/imgui_impl_win32.h"

using namespace Microsoft::WRL;

namespace Insight {



	Direct3D12Context::Direct3D12Context(WindowsWindow* windowHandle)
		: m_pWindowHandle(&windowHandle->GetWindowHandleReference()), m_pWindow(windowHandle), RenderingContext(windowHandle->GetWidth(), windowHandle->GetHeight(), false)
	{
		IE_CORE_ASSERT(windowHandle, "Window handle is NULL!");
	}

	Direct3D12Context::~Direct3D12Context()
	{
		Cleanup();
	}

	bool Direct3D12Context::Init()
	{
		try {
			CreateDXGIFactory();
			CreateDevice();

			CreateCommandQueue();
			CreateSwapChain();
			CreateViewport();
			CreateScissorRect();

			CreateDescriptorHeaps();
			
			CreateDepthStencilBuffer();
			
			CreateFenceEvent();
			CreateCommandAllocators();


			CreatePipelineStateObjects();
			
			CrateConstantBufferResourceHeaps();
			CreateCamera();

			m_pCommandList->Close();
			ID3D12CommandList* ppCommandLists[] = { m_pCommandList.Get() };
			m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

			m_FenceValue[m_FrameIndex]++;
			HRESULT hr = m_pCommandQueue->Signal(m_pFence->GetAddressOf()[m_FrameIndex], m_FenceValue[m_FrameIndex]);
			COM_ERROR_IF_FAILED(hr, "Failed to signal command queue when uploading vertex buffer");

		}
		catch (COMException& ex) {
			COM_SAFE_RELEASE(m_pDxgiFactory);
			MessageBox(*m_pWindowHandle, ex.what(), L"Error", MB_OK);
			__debugbreak();
			return false;
		}
		return true;
	}

	void Direct3D12Context::OnUpdate()
	{
		using namespace DirectX;
		
		XMMATRIX rotXMat = XMMatrixRotationX(0.0001f);
		XMMATRIX rotYMat = XMMatrixRotationY(0.0002f);
		XMMATRIX rotZMat = XMMatrixRotationZ(0.0003f);

		XMMATRIX rotMat = XMLoadFloat4x4(&cube1RotMat) * rotXMat * rotYMat * rotZMat;
		XMStoreFloat4x4(&cube1RotMat, rotMat);

		XMMATRIX translationMat = XMMatrixTranslationFromVector(XMLoadFloat4(&cube1Position));

		XMMATRIX worldMat = rotMat * translationMat;

		XMStoreFloat4x4(&cube1WorldMat, worldMat);

		XMMATRIX viewMat = XMLoadFloat4x4(&cameraViewMat); // load view matrix
		XMMATRIX projMat = XMLoadFloat4x4(&cameraProjMat); // load projection matrix
		XMMATRIX wvpMat = XMLoadFloat4x4(&cube1WorldMat) * viewMat * projMat; // create wvp matrix
		XMMATRIX transposed = XMMatrixTranspose(wvpMat); // must transpose wvp matrix for the gpu
		XMStoreFloat4x4(&cbPerObject.wvpMatrix, transposed); // store transposed wvp matrix in constant buffer

		memcpy(cbvGPUAddress[m_FrameIndex], &cbPerObject, sizeof(cbPerObject));

		rotXMat = XMMatrixRotationX(0.0003f);
		rotYMat = XMMatrixRotationY(0.0002f);
		rotZMat = XMMatrixRotationZ(0.0001f);

		rotMat = rotZMat * (XMLoadFloat4x4(&cube2RotMat) * (rotXMat * rotYMat));
		XMStoreFloat4x4(&cube2RotMat, rotMat);

		XMMATRIX translationOffsetMat = XMMatrixTranslationFromVector(XMLoadFloat4(&cube2PositionOffset));

		XMMATRIX scaleMat = XMMatrixScaling(0.5f, 0.5f, 0.5f);

		worldMat = scaleMat * translationOffsetMat * rotMat * translationMat;

		wvpMat = XMLoadFloat4x4(&cube2WorldMat) * viewMat * projMat; // create wvp matrix
		transposed = XMMatrixTranspose(wvpMat); // must transpose wvp matrix for the gpu
		XMStoreFloat4x4(&cbPerObject.wvpMatrix, transposed); // store transposed wvp matrix in constant buffer

		memcpy(cbvGPUAddress[m_FrameIndex] + ConstantBufferPerObjectAlignedSize, &cbPerObject, sizeof(cbPerObject));

		// store cube2's world matrix
		XMStoreFloat4x4(&cube2WorldMat, worldMat);
	}

	void Direct3D12Context::WaitForPreviousFrame()
	{
		HRESULT hr;

		// swap the current rtv buffer index so we draw on the correct buffer
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		// if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
		// the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
		if (m_pFence[m_FrameIndex]->GetCompletedValue() < m_FenceValue[m_FrameIndex])
		{
			// we have the fence create an event which is signaled once the fence's current value is "fenceValue"
			hr = m_pFence[m_FrameIndex]->SetEventOnCompletion(m_FenceValue[m_FrameIndex], m_FenceEvent);
			//COM_ERROR_IF_FAILED(hr, "Failed to set event completion value while wiating for frame:");


			// We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
			// has reached "fenceValue", we know the command queue has finished executing
			WaitForSingleObject(m_FenceEvent, INFINITE);
		}

		m_FenceValue[m_FrameIndex]++;
	}

	void Direct3D12Context::PopulateCommandLists()
	{
		HRESULT hr;

		WaitForPreviousFrame();
		hr = m_pCommandAllocators[m_FrameIndex]->Reset();
		if (FAILED(hr))
		{
			throw std::exception();
		}

		hr = m_pCommandList->Reset(m_pCommandAllocators[m_FrameIndex].Get(), m_pPipelineStateObject_Default.Get());
		if (FAILED(hr))
		{
			throw std::exception();
		}

		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_pDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);

		m_pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

		const float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		m_pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		m_pCommandList->ClearDepthStencilView(m_pDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		m_pCommandList->SetGraphicsRootSignature(m_pRootSignature_Default.Get());
		m_pCommandList->RSSetViewports(1, &m_ViewPort);
		m_pCommandList->RSSetScissorRects(1, &m_ScissorRect);

		m_pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// set root signature
		m_pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
		m_pCommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView); // set the vertex buffer (using the vertex buffer view)
		m_pCommandList->IASetIndexBuffer(&m_IndexBufferView);

		// first cube

		// set cube1's constant buffer
		m_pCommandList->SetGraphicsRootConstantBufferView(0, constantBufferUploadHeaps[m_FrameIndex]->GetGPUVirtualAddress());

		// draw first cube
		m_pCommandList->DrawIndexedInstanced(numCubeIndices, 1, 0, 0, 0);

		// second cube

		// set cube2's constant buffer. You can see we are adding the size of ConstantBufferPerObject to the constant buffer
		// resource heaps address. This is because cube1's constant buffer is stored at the beginning of the resource heap, while
		// cube2's constant buffer data is stored after (256 bits from the start of the heap).
		m_pCommandList->SetGraphicsRootConstantBufferView(0, constantBufferUploadHeaps[m_FrameIndex]->GetGPUVirtualAddress() + ConstantBufferPerObjectAlignedSize);

		// draw second cube
		m_pCommandList->DrawIndexedInstanced(numCubeIndices, 1, 0, 0, 0);

		
		// Render ImGui UI
		m_pCommandList->SetDescriptorHeaps(1, m_pImGuiDescriptorHeap.GetAddressOf());
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_pCommandList.Get());


		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		hr = m_pCommandList->Close();
		if (FAILED(hr))
		{
			throw std::exception();
		}

	}

	void Direct3D12Context::RenderFrame()
	{
		HRESULT hr;

		PopulateCommandLists();

		ID3D12CommandList* ppCommandLists[] = { m_pCommandList.Get() };

		m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		hr = m_pCommandQueue->Signal(m_pFence[m_FrameIndex].Get(), m_FenceValue[m_FrameIndex]);

	}

	void Direct3D12Context::SwapBuffers()
	{
		m_pSwapChain->Present(m_VSyncEnabled, 0);
	}

	void Direct3D12Context::OnWindowResize(UINT width, UINT height)
	{
		WaitForGPU();

		for (UINT i = 0; i < m_FrameBufferCount; i++)
		{
			m_pRenderTargets[i].Reset();
			m_FenceValue[i] = m_FenceValue[m_FrameIndex];
		}

		DXGI_SWAP_CHAIN_DESC desc = {};
		m_pSwapChain->GetDesc(&desc);
	
		HRESULT hr = m_pSwapChain->ResizeBuffers(m_FrameBufferCount, width, height, desc.BufferDesc.Format, desc.Flags);
		if (FAILED(hr))
			__debugbreak();

		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		for (UINT i = 0; i < m_FrameBufferCount; i++)
		{
			m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargets[i]));
			m_pDevice->CreateRenderTargetView(m_pRenderTargets[i].Get(), nullptr, rtvHandle);
		}

		/*CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_pDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&m_pDepthStencilBuffer));*/

	}

	void Direct3D12Context::CreateSwapChain()
	{
		HRESULT hr;

		DXGI_MODE_DESC backBufferDesc = {};
		backBufferDesc.Width = m_pWindow->GetWidth();
		backBufferDesc.Height = m_pWindow->GetHeight();

		m_SampleDesc = {};
		m_SampleDesc.Count = 1;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = m_FrameBufferCount;
		swapChainDesc.Width = m_pWindow->GetWidth();
		swapChainDesc.Height = m_pWindow->GetHeight();
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc = m_SampleDesc;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain{};
		
		hr = m_pDxgiFactory->CreateSwapChainForHwnd(m_pCommandQueue.Get(), *m_pWindowHandle, &swapChainDesc, nullptr, nullptr, &swapChain);
		COM_ERROR_IF_FAILED(hr, "Failed to Create Swap Chain");
		
		hr = m_pDxgiFactory->MakeWindowAssociation(*m_pWindowHandle, DXGI_MWA_NO_ALT_ENTER);
		COM_ERROR_IF_FAILED(hr, "Failed to Make Window Association");
		
		hr = swapChain.As(&m_pSwapChain);
		COM_ERROR_IF_FAILED(hr, "Failed to cast SwapChain ComPtr");
		
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	}

	void Direct3D12Context::CreateDescriptorHeaps()
	{
		CreateRTVDescriptorHeap();
		CreateDSVDescriptorHeap();
		CreateImGuiDescriptorHeap();
	}

	void Direct3D12Context::CreateRTVDescriptorHeap()
	{
		HRESULT hr;
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = m_FrameBufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		hr = m_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_pRtvDescriptorHeap));
		COM_ERROR_IF_FAILED(hr, "Failed to Create Descriptor Heap");

		m_RtvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		for (int i = 0; i < m_FrameBufferCount; i++)
		{
			hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargets[i]));
			COM_ERROR_IF_FAILED(hr, "Failed to initialize Render Targets");

			m_pDevice->CreateRenderTargetView(m_pRenderTargets[i].Get(), nullptr, rtvHandle);

			rtvHandle.Offset(1, m_RtvDescriptorSize);
		}
	}

	void Direct3D12Context::CreateDSVDescriptorHeap()
	{
		HRESULT hr;
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		hr = m_pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_pDepthStencilDescriptorHeap.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "Failed to create descriptor heap for Depth Stencil View.");
	}

	void Direct3D12Context::CreateDepthStencilBuffer()
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC dsDesc = {};
		dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsDesc.Flags = D3D12_DSV_FLAG_NONE;

		D3D12_CLEAR_VALUE depthOptomizedClearValue = {};
		depthOptomizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		depthOptomizedClearValue.DepthStencil.Depth = 1.0f;
		depthOptomizedClearValue.DepthStencil.Stencil = 0;

		m_pDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_pWindow->GetWidth(), m_pWindow->GetHeight(), 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptomizedClearValue,
			IID_PPV_ARGS(m_pDepthStencilBuffer.GetAddressOf()));

		m_pDepthStencilDescriptorHeap->SetName(L"Depth/Stencil Resource Heap");

		m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), &dsDesc, m_pDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		
	}

	void Direct3D12Context::CreateCommandAllocators()
	{
		HRESULT hr;
		for (int i = 0; i < m_FrameBufferCount; i++)
		{
			hr = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocators[i]));
			COM_ERROR_IF_FAILED(hr, "Failed to Create Command Allocator");
		}

		hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pCommandList));
		COM_ERROR_IF_FAILED(hr, "Failed to Create Command List");

	}

	void Direct3D12Context::CreateFenceEvent()
	{
		HRESULT hr;
		for (int i = 0; i < m_FrameBufferCount; i++)
		{
			hr = m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence[i]));
			COM_ERROR_IF_FAILED(hr, "Failed to create Fence on index" + std::to_string(i));

			m_FenceValue[i] = 0;
		}

		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_FenceEvent == nullptr)
			THROW_COM_ERROR("Fence Event was nullptr");
	}

	void Direct3D12Context::CreatePipelineStateObjects()
	{
		HRESULT hr;

		D3D12_ROOT_DESCRIPTOR rootCBVDescriptor = {};
		rootCBVDescriptor.RegisterSpace = 0;
		rootCBVDescriptor.ShaderRegister = 0;

		D3D12_ROOT_PARAMETER rootParameters[1];
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;;
		rootParameters[0].Descriptor = rootCBVDescriptor;
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);

		ID3DBlob* RootSignatureByteCode = nullptr;
		hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &RootSignatureByteCode, nullptr);
		COM_ERROR_IF_FAILED(hr, "Failed to serialize Root Signature");

		hr = m_pDevice->CreateRootSignature(0, RootSignatureByteCode->GetBufferPointer(), RootSignatureByteCode->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature_Default));
		COM_ERROR_IF_FAILED(hr, "Failed to create Default Root Signature");

		// TODO: Make Shader class
		// TODO: Move this to the shader class
		// Compile vertex shader // TEMP//
#pragma region Make this a Vertex class
		// Note: Searches for shaders relative to Application.vcxproj
		ID3DBlob* pVertexShader = nullptr;
		ID3DBlob* pErrorBuffer = nullptr;
		//TODO: make a file system search for this relative sahder path in client
		// The client should be able to edit and create their own shaders
		hr = D3DCompileFromFile(L"src/Shaders/ForwardRendering/Shader_Vertex.hlsl",
			nullptr,
			nullptr,
			"main",
			"vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&pVertexShader,
			&pErrorBuffer);
		if (FAILED(hr))
		{
			COM_ERROR_IF_FAILED(hr, "Failed to compile Vertex Shader check log for more details.");
			IE_CORE_ERROR("Vertex Shader compilation error: {0}", (char*)pErrorBuffer->GetBufferPointer());
		}
		D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
		vertexShaderBytecode.BytecodeLength = pVertexShader->GetBufferSize();
		vertexShaderBytecode.pShaderBytecode = pVertexShader->GetBufferPointer();
		// Compile pixel shader // TEMP//
		// create vertex and pixel shaders
		ID3DBlob* pixelShader = nullptr;
		hr = D3DCompileFromFile(L"src/Shaders/ForwardRendering/Shader_Pixel.hlsl",
			nullptr,
			nullptr,
			"main",
			"ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&pixelShader,
			&pErrorBuffer);
		if (FAILED(hr))
		{
			IE_CORE_ERROR("Pixel Shader compilation error: {0}", (char*)pErrorBuffer->GetBufferPointer());
			COM_ERROR_IF_FAILED(hr, "Failed to compile Pixel Shader check log for more details.");
		}
		D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
		pixelShaderBytecode.BytecodeLength = pixelShader->GetBufferSize();
		pixelShaderBytecode.pShaderBytecode = pixelShader->GetBufferPointer();

		// Create Input layout 
		D3D12_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
		inputLayoutDesc.pInputElementDescs = inputLayout;
#pragma endregion

		// Create Pipeline State Object
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = inputLayoutDesc;
		psoDesc.pRootSignature = m_pRootSignature_Default.Get();
		psoDesc.VS = vertexShaderBytecode;
		psoDesc.PS = pixelShaderBytecode;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc = m_SampleDesc;
		psoDesc.SampleMask = 0xffffffff;
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.NumRenderTargets = 1;

		hr = m_pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pPipelineStateObject_Default));
		COM_ERROR_IF_FAILED(hr, "Failed to create default Pipeline State Object");

		// TODO Make model class
		// TODO: move thi to the model class
		Vertex vList[] = {
			// front face
			{ -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
			{  0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
			{ -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
			{  0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f },

			// right side face
			{  0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
			{  0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
			{  0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
			{  0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f },

			// left side face
			{ -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
			{ -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
			{ -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f },

			// back face
			{  0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
			{  0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
			{ -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f },

			// top face
			{ -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
			{ 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
			{ -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f },

			// bottom face
			{  0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f },
			{  0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
			{ -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f },
		};
		int vBufferSize = sizeof(vList);

		hr = m_pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_pVertexBuffer));
		m_pVertexBuffer->SetName(L"Vertex Buffer Resource Heap");
		COM_ERROR_IF_FAILED(hr, "Failed to upload vertex buffer resource heap");

		hr = m_pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_pVBufferUploadHeap));
		m_pVBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");
		COM_ERROR_IF_FAILED(hr, "Failed to upload vertex buffer heap");

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<BYTE*>(vList);
		vertexData.RowPitch = vBufferSize;
		vertexData.SlicePitch = vBufferSize;

		UpdateSubresources(m_pCommandList.Get(), m_pVertexBuffer.Get(), m_pVBufferUploadHeap.Get(), 0, 0, 1, &vertexData);

		m_VertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = sizeof(Vertex);
		m_VertexBufferView.SizeInBytes = vBufferSize;

		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));


		// Create Index Buffer
		DWORD iList[] = {
			// front face
		   0, 1, 2, // first triangle
		   0, 3, 1, // second triangle

		   // left face
		   4, 5, 6, // first triangle
		   4, 7, 5, // second triangle

		   // right face
		   8, 9, 10, // first triangle
		   8, 11, 9, // second triangle

		   // back face
		   12, 13, 14, // first triangle
		   12, 15, 13, // second triangle

		   // top face
		   16, 17, 18, // first triangle
		   16, 19, 17, // second triangle

		   // bottom face
		   20, 21, 22, // first triangle
		   20, 23, 21, // second triangle
		};
		int iBufferSize = sizeof(iList);
		hr = m_pDevice->CreateCommittedResource(
												&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
												D3D12_HEAP_FLAG_NONE,
												&CD3DX12_RESOURCE_DESC::Buffer(iBufferSize),
												D3D12_RESOURCE_STATE_COPY_DEST,
												nullptr,
												IID_PPV_ARGS(m_pIndexBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "Failed to create Committed Resource for Index Buffer to the Deafault Heap");
		m_pIndexBuffer->SetName(L"Index Buffer Resource Heap");

		hr = m_pDevice->CreateCommittedResource(
												&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
												D3D12_HEAP_FLAG_NONE,
												&CD3DX12_RESOURCE_DESC::Buffer(iBufferSize),
												D3D12_RESOURCE_STATE_GENERIC_READ,
												nullptr,
												IID_PPV_ARGS(m_pIndexBufferUploadHeap.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "Failed to create Committed Resource for Index Buffer to the Upload Heap");
		
		m_pIndexBufferUploadHeap->SetName(L"Index Buffer Upload Resource Heap");
		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = reinterpret_cast<BYTE*>(iList);
		indexData.RowPitch = iBufferSize;
		indexData.SlicePitch = iBufferSize;

		UpdateSubresources(m_pCommandList.Get(), m_pIndexBuffer.Get(), m_pIndexBufferUploadHeap.Get(), 0, 0, 1, &indexData);
		
		m_IndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = iBufferSize;

		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));

		
	}

	void Direct3D12Context::CrateConstantBufferResourceHeaps()
	{
		HRESULT hr;

		for (int i = 0; i < m_FrameBufferCount; ++i)
		{
			hr = m_pDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE, 
				&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
				D3D12_RESOURCE_STATE_GENERIC_READ, 
				nullptr, 
				IID_PPV_ARGS(&constantBufferUploadHeaps[i]));
			constantBufferUploadHeaps[i]->SetName(L"Constant Buffer Upload Resource Heap");

			ZeroMemory(&cbPerObject, sizeof(cbPerObject));

			CD3DX12_RANGE readRange(0, 0);    

			hr = constantBufferUploadHeaps[i]->Map(0, &readRange, reinterpret_cast<void**>(&cbvGPUAddress[i]));

			memcpy(cbvGPUAddress[i], &cbPerObject, sizeof(cbPerObject)); 
			memcpy(cbvGPUAddress[i] + ConstantBufferPerObjectAlignedSize, &cbPerObject, sizeof(cbPerObject)); 
		}
	}

	void Direct3D12Context::CreateViewport()
	{
		m_ViewPort.TopLeftX = 0;
		m_ViewPort.TopLeftY = 0;
		m_ViewPort.Width = static_cast<FLOAT>(m_pWindow->GetWidth());
		m_ViewPort.Height = static_cast<FLOAT>(m_pWindow->GetHeight());
		m_ViewPort.MinDepth = 0.0f;
		m_ViewPort.MaxDepth = 1.0f;
	}

	void Direct3D12Context::CreateScissorRect()
	{
		m_ScissorRect.left = 0;
		m_ScissorRect.top = 0;
		m_ScissorRect.right = m_pWindow->GetWidth();
		m_ScissorRect.bottom = m_pWindow->GetHeight();
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

	void Direct3D12Context::CreateCamera()
	{
		using namespace DirectX;

		XMMATRIX tmpMat = XMMatrixPerspectiveFovLH(45.0f * (3.14f / 180.0f), (float)m_WindowWidth / (float)m_WindowHeight, 0.1f, 1000.0f);
		XMStoreFloat4x4(&cameraProjMat, tmpMat);

		cameraPosition = XMFLOAT4(0.0f, 2.0f, -4.0f, 0.0f);
		cameraTarget = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		cameraUp = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		XMVECTOR cPos = XMLoadFloat4(&cameraPosition);
		XMVECTOR cTarg = XMLoadFloat4(&cameraTarget);
		XMVECTOR cUp = XMLoadFloat4(&cameraUp);
		tmpMat = XMMatrixLookAtLH(cPos, cTarg, cUp);
		XMStoreFloat4x4(&cameraViewMat, tmpMat);

		cube1Position = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR posVec = XMLoadFloat4(&cube1Position);

		tmpMat = XMMatrixTranslationFromVector(posVec); 
		XMStoreFloat4x4(&cube1RotMat, XMMatrixIdentity()); 
		XMStoreFloat4x4(&cube1WorldMat, tmpMat); 

		cube2PositionOffset = XMFLOAT4(1.5f, 0.0f, 0.0f, 0.0f);
		posVec = XMLoadFloat4(&cube2PositionOffset) + XMLoadFloat4(&cube1Position); 
																					

		tmpMat = XMMatrixTranslationFromVector(posVec); 
		XMStoreFloat4x4(&cube2RotMat, XMMatrixIdentity());
		XMStoreFloat4x4(&cube2WorldMat, tmpMat);
	}

	void Direct3D12Context::CreateCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		HRESULT hr = m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCommandQueue));
		COM_ERROR_IF_FAILED(hr, "Failed to Create Command Queue");
	}

	void Direct3D12Context::CreateImGuiDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pImGuiDescriptorHeap));
	}

	void Direct3D12Context::CreateDevice()
	{
		HRESULT hr;
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(m_pDxgiFactory.Get(), &hardwareAdapter);
		hr = D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_pDevice));
		COM_ERROR_IF_FAILED(hr, "Failed to create hardware adapter.");
	}

	void Direct3D12Context::CreateDXGIFactory()
	{
		UINT dxgiFactoryFlags = 0;
		// Enable debug layers
#ifdef IE_DEBUG
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif

		HRESULT hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_pDxgiFactory));
		COM_ERROR_IF_FAILED(hr, "Failed to create DXGI Factory");

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
				continue; // We dont use software adapters

			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), m_RayTraceEnabled ? D3D_FEATURE_LEVEL_12_1 : D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
				break;
		}
		*ppAdapter = adapter.Detach();
	}

	void Direct3D12Context::Cleanup()
	{
		WaitForPreviousFrame();
		CloseHandle(m_FenceEvent);

		for (int i = 0; i < m_FrameBufferCount; i++)
		{
			m_FrameIndex = i;
			WaitForPreviousFrame();
		}

		BOOL fs = false;
		if (m_pSwapChain->GetFullscreenState(&fs, NULL))
			m_pSwapChain->SetFullscreenState(false, NULL);


	}

	void Direct3D12Context::WaitForGPU()
	{
		m_pCommandQueue->Signal(m_pFence[m_FrameIndex].Get(), m_FenceValue[m_FrameIndex]);

		// Wait until the fence has been processed.
		m_pFence[m_FrameIndex].Get()->SetEventOnCompletion(m_FenceValue[m_FrameIndex], m_FenceEvent);
		WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

		// Increment the fence value for the current frame.
		m_FenceValue[m_FrameIndex]++;
	}

	void Direct3D12Context::ToggleFullscreen(IDXGISwapChain* pSwapChain)
	{
		if (m_pWindow->GetIsWindowFullScreen())
		{
			//SetWindowLong(*m_pWindowHandle, GWL_STYLE);

		}
	}

}