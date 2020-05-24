#include <ie_pch.h>

#include "Direct3D12_Context.h"

#include "Platform/Windows/Windows_Window.h"
#include "Insight/Input/Input.h"
#include "Insight/Core/Application.h"
#include "Insight/Runtime/APlayer_Character.h"

#include "imgui.h"
#include "examples/imgui_impl_dx12.h"

namespace Insight {

	Direct3D12Context* Direct3D12Context::s_Instance = nullptr;

	const Direct3D12Context::Resolution Direct3D12Context::m_ResolutionOptions[] =
	{
		{ 800u, 600u },
		{ 1200u, 900u },
		{ 1280u, 720u },
		{ 1920u, 1080u },
		{ 1920u, 1200u },
		{ 2560u, 1440u },
		{ 3440u, 1440u },
		{ 3840u, 2160u }
	};
	const UINT Direct3D12Context::m_ResolutionOptionsCount = _countof(m_ResolutionOptions);
	UINT Direct3D12Context::m_ResolutionIndex = 2;

	Direct3D12Context::Direct3D12Context(WindowsWindow* windowHandle)
		: m_pWindowHandle(&windowHandle->GetWindowHandleReference()), m_pWindow(windowHandle), RenderingContext(windowHandle->GetWidth(), windowHandle->GetHeight(), false)
	{
		IE_CORE_ASSERT(windowHandle, "Window handle is NULL!");
		IE_ASSERT(!s_Instance, "Rendering instance already exists!");
		s_Instance = this;

		m_AspectRatio = (float)m_WindowWidth / (float)m_WindowHeight;
		camera.SetProjectionValues(75.0f, m_AspectRatio, 0.0001f, 1000.0f);
	}

	Direct3D12Context::~Direct3D12Context()
	{
		Cleanup();
	}

	void Direct3D12Context::Cleanup()
	{
		// Wait until GPU is done consuming all data
		// before closing all handles and releasing resources
		WaitForGPU();

		if (!m_AllowTearing)
			m_pSwapChain->SetFullscreenState(false, NULL);

		CloseHandle(m_FenceEvent);
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

			CreateConstantBufferResourceHeaps();
			
			//LoadAssets();

			//CloseCommandListAndSignalCommandQueue();

		}
		catch (COMException& ex) {
			COM_SAFE_RELEASE(m_pDxgiFactory);
			MessageBox(*m_pWindowHandle, ex.what(), L"Error", MB_OK);
			return false;
		}
		return true;
	}

	void Direct3D12Context::OnUpdate(const float& deltaTime)
	{
		
		// TODO: move this to player controller
		if (Input::IsMouseButtonPressed(IE_MOUSEBUTTON_RIGHT))
		{
			auto [x, y] = Input::GetRawMousePosition();
			camera.ProcessMouseMovement((float)x, (float)y);
		}
		if (Input::IsKeyPressed('W'))
			camera.ProcessKeyboardInput(CameraMovement::FORWARD, deltaTime);
		if (Input::IsKeyPressed('A'))
			camera.ProcessKeyboardInput(CameraMovement::LEFT, deltaTime);
		if (Input::IsKeyPressed('S'))
			camera.ProcessKeyboardInput(CameraMovement::BACKWARD, deltaTime);
		if (Input::IsKeyPressed('D'))
			camera.ProcessKeyboardInput(CameraMovement::RIGHT, deltaTime);
		if (Input::IsKeyPressed('E'))
			camera.ProcessKeyboardInput(CameraMovement::UP, deltaTime);
		if (Input::IsKeyPressed('Q'))
			camera.ProcessKeyboardInput(CameraMovement::DOWN, deltaTime);
		
		m_PerFrameData.cameraPosition = camera.GetTransformRef().GetPosition();
		memcpy(m_cbvPerFrameGPUAddress[m_FrameIndex], &m_PerFrameData, sizeof(m_PerFrameData));
	}

	void Direct3D12Context::WaitForPreviousFrame()
	{
		HRESULT hr;

		// swap the current rtv buffer index so we draw on the correct buffer
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		// if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
		// the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
		if (m_pFences[m_FrameIndex]->GetCompletedValue() < m_FenceValues[m_FrameIndex])
		{
			// we have the fence create an event which is signaled once the fence's current value is "fenceValue"
			hr = m_pFences[m_FrameIndex]->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_FenceEvent);
			COM_ERROR_IF_FAILED(hr, "Failed to set event completion value while waiting for frame");


			// We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
			// has reached "fenceValue", we know the command queue has finished executing
			WaitForSingleObject(m_FenceEvent, INFINITE);
		}

		m_FenceValues[m_FrameIndex]++;
	}

	void Direct3D12Context::OnPreFrameRender()
	{
		HRESULT hr;
		WaitForPreviousFrame();

		hr = m_pCommandAllocators[m_FrameIndex]->Reset();
		if (FAILED(hr))
			throw std::exception();

		hr = m_pCommandList->Reset(m_pCommandAllocators[m_FrameIndex].Get(), m_pPipelineStateObject_ForwardPass.Get());
		if (FAILED(hr))
			throw std::exception();
	}

	void Direct3D12Context::PopulateCommandLists()
	{

		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_pDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		m_pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

		const float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		m_pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		m_pCommandList->ClearDepthStencilView(m_pDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		m_pCommandList->SetGraphicsRootSignature(m_pRootSignature_ForwardPass.Get());

		m_pCommandList->RSSetViewports(1, &m_ViewPort);
		m_pCommandList->RSSetScissorRects(1, &m_ScissorRect);
		m_pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ID3D12DescriptorHeap* descriptorHeaps[] = { m_pMainDescriptorHeap.Get() };
		m_pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		//albedoTexture.Bind();
		//normalTexture.Bind();
		//specularTexture.Bind();

		m_pCommandList->SetGraphicsRootConstantBufferView(1, m_ConstantBufferPerFrameUploadHeaps[m_FrameIndex]->GetGPUVirtualAddress());


	}

	void Direct3D12Context::OnRender()
	{
		if (m_WindowVisible)
		{
			PopulateCommandLists();
		}
	}

	void Direct3D12Context::ExecuteDraw()
	{
		HRESULT hr;

		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		hr = m_pCommandList->Close();
		if (FAILED(hr)) {
			IE_CORE_FATAL(L"Failed to close command list. Cannot execute draw commands");
		}

		ID3D12CommandList* ppCommandLists[] = { m_pCommandList.Get() };
		m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		hr = m_pCommandQueue->Signal(m_pFences[m_FrameIndex].Get(), m_FenceValues[m_FrameIndex]);
		if (FAILED(hr))
			IE_CORE_WARN("Command queue failed to signal.");
	}

	void Direct3D12Context::SwapBuffers()
	{
		UINT presentFlags = (m_AllowTearing && m_WindowedMode) ? DXGI_PRESENT_ALLOW_TEARING : 0;
		m_pSwapChain->Present(m_VSyncEnabled, presentFlags);
	}

	void Direct3D12Context::OnWindowResize()
	{
		if (!m_IsMinimized)
		{
			if (m_WindowResizeComplete)
			{
				m_WindowResizeComplete = false;

				WaitForGPU();
				HRESULT hr;

				for (UINT i = 0; i < m_FrameBufferCount; i++)
				{
					m_pRenderTargets[i].Reset();
					m_FenceValues[i] = m_FenceValues[m_FrameIndex];
				}
				m_pDepthStencilBuffer.Reset();

				DXGI_SWAP_CHAIN_DESC desc = {};
				m_pSwapChain->GetDesc(&desc);
				hr = m_pSwapChain->ResizeBuffers(m_FrameBufferCount, m_WindowWidth, m_WindowHeight, desc.BufferDesc.Format, desc.Flags);
				if (FAILED(hr))
					__debugbreak();

				BOOL fullScreenState;
				m_pSwapChain->GetFullscreenState(&fullScreenState, nullptr);
				m_WindowedMode = !fullScreenState;

				m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

				UpdateSizeDependentResources();
			}

		}
		m_WindowVisible = !m_IsMinimized;
		m_WindowResizeComplete = true;
	}

	void Direct3D12Context::OnWindowFullScreen()
	{
		if (m_FullScreenMode)
		{
			SetWindowLong(*m_pWindowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);

			SetWindowPos(
				*m_pWindowHandle,
				HWND_NOTOPMOST,
				m_pWindow->GetWindowRect().left,
				m_pWindow->GetWindowRect().top,
				m_pWindow->GetWindowRect().right - m_pWindow->GetWindowRect().left,
				m_pWindow->GetWindowRect().bottom - m_pWindow->GetWindowRect().top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE
			);
			ShowWindow(*m_pWindowHandle, SW_NORMAL);
		}
		else
		{
			GetWindowRect(*m_pWindowHandle, &m_pWindow->GetWindowRect());

			SetWindowLong(*m_pWindowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

			RECT fullscreenWindowRect;
			try
			{
				if (m_pSwapChain)
				{
					// Get the settings of the display on which the app's window is currently displayed
					ComPtr<IDXGIOutput> pOutput;
					COM_ERROR_IF_FAILED(m_pSwapChain->GetContainingOutput(&pOutput), "Failed to get containing output");
					DXGI_OUTPUT_DESC Desc;
					COM_ERROR_IF_FAILED(pOutput->GetDesc(&Desc), "Failed to get description from output");
					fullscreenWindowRect = Desc.DesktopCoordinates;
				}
				else
				{
					// Fallback to EnumDisplaySettings implementation
					throw COMException(NULL, "No Swap chain available", __FILE__, __FUNCTION__, __LINE__);
				}
			}
			catch (COMException& e)
			{
				UNREFERENCED_PARAMETER(e);

				// Get the settings of the primary display
				DEVMODE devMode = {};
				devMode.dmSize = sizeof(DEVMODE);
				EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);

				fullscreenWindowRect = {
					devMode.dmPosition.x,
					devMode.dmPosition.y,
					devMode.dmPosition.x + static_cast<LONG>(devMode.dmPelsWidth),
					devMode.dmPosition.y + static_cast<LONG>(devMode.dmPelsHeight)
				};
			}

			SetWindowPos(
				*m_pWindowHandle,
				HWND_TOPMOST,
				fullscreenWindowRect.left,
				fullscreenWindowRect.top,
				fullscreenWindowRect.right,
				fullscreenWindowRect.bottom,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);


			ShowWindow(*m_pWindowHandle, SW_MAXIMIZE);
		}
		m_FullScreenMode = !m_FullScreenMode;
	}

	void Direct3D12Context::CreateSwapChain()
	{
		HRESULT hr;

		DXGI_MODE_DESC backBufferDesc = {};
		backBufferDesc.Width = m_WindowWidth;
		backBufferDesc.Height = m_WindowHeight;

		m_SampleDesc = {};
		m_SampleDesc.Count = 1;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = m_FrameBufferCount;
		swapChainDesc.Width = m_WindowWidth;
		swapChainDesc.Height = m_WindowHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc = m_SampleDesc;
		swapChainDesc.Flags = m_AllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain{};

		hr = m_pDxgiFactory->CreateSwapChainForHwnd(m_pCommandQueue.Get(), *m_pWindowHandle, &swapChainDesc, nullptr, nullptr, &swapChain);
		COM_ERROR_IF_FAILED(hr, "Failed to Create Swap Chain");

		if (m_AllowTearing)
		{
			hr = m_pDxgiFactory->MakeWindowAssociation(*m_pWindowHandle, DXGI_MWA_NO_ALT_ENTER);
			COM_ERROR_IF_FAILED(hr, "Failed to Make Window Association");
		}

		hr = swapChain.As(&m_pSwapChain);
		COM_ERROR_IF_FAILED(hr, "Failed to cast SwapChain ComPtr");

		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	}

	void Direct3D12Context::CreateDescriptorHeaps()
	{
		CreateRenderTargetViewDescriptorHeap();
		CreateDepthStencilViewDescriptorHeap();
		CreateShaderVisibleResourceDescriptorHeap();
	}

	void Direct3D12Context::CreateRenderTargetViewDescriptorHeap()
	{
		HRESULT hr;
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = m_FrameBufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		hr = m_pLogicalDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_pRtvDescriptorHeap));
		COM_ERROR_IF_FAILED(hr, "Failed to Create Descriptor Heap");

		m_RtvDescriptorSize = m_pLogicalDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		for (int i = 0; i < m_FrameBufferCount; i++)
		{
			hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargets[i]));
			COM_ERROR_IF_FAILED(hr, "Failed to initialize Render Targets");

			m_pLogicalDevice->CreateRenderTargetView(m_pRenderTargets[i].Get(), nullptr, rtvHandle);

			rtvHandle.Offset(1, m_RtvDescriptorSize);
		}
	}

	void Direct3D12Context::CreateDepthStencilViewDescriptorHeap()
	{
		HRESULT hr;
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		hr = m_pLogicalDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_pDepthStencilDescriptorHeap.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "Failed to create descriptor heap for Depth Stencil View.");
		m_pDepthStencilDescriptorHeap->SetName(L"Depth/Stencil Resource Heap");

	}

	void Direct3D12Context::CreateShaderVisibleResourceDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.NumDescriptors = 25; //TODO: Get this from the number of textures held in texture manager (TextureManager::GetNumTextures() or something)
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		HRESULT hr = m_pLogicalDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pMainDescriptorHeap));
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to create descriptor heap");
		}
		m_MainDescriptorHeapHandleWithOffset = m_pMainDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	}

	void Direct3D12Context::CreateDepthStencilBuffer()
	{
		HRESULT hr;

		m_dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		m_dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		m_dsvDesc.Texture2D.MipSlice = 0;
		m_dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		D3D12_CLEAR_VALUE depthOptomizedClearValue = {};
		depthOptomizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		depthOptomizedClearValue.DepthStencil.Depth = 1.0f;
		depthOptomizedClearValue.DepthStencil.Stencil = 0;

		hr = m_pLogicalDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_WindowWidth, m_WindowHeight, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptomizedClearValue,
			IID_PPV_ARGS(&m_pDepthStencilBuffer));
		if (FAILED(hr))
			IE_CORE_ERROR("Failed to create comitted resource for depth stencil view");

		m_pLogicalDevice->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), &m_dsvDesc, m_pDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	}

	void Direct3D12Context::CreateCommandAllocators()
	{
		HRESULT hr;
		for (int i = 0; i < m_FrameBufferCount; i++)
		{
			hr = m_pLogicalDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocators[i]));
			COM_ERROR_IF_FAILED(hr, "Failed to Create Command Allocator");
		}

		hr = m_pLogicalDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pCommandList));
		COM_ERROR_IF_FAILED(hr, "Failed to Create Command List");
	}

	void Direct3D12Context::CreateFenceEvent()
	{
		HRESULT hr;
		for (int i = 0; i < m_FrameBufferCount; i++)
		{
			hr = m_pLogicalDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFences[i]));
			COM_ERROR_IF_FAILED(hr, "Failed to create Fence on index" + std::to_string(i));

			m_FenceValues[i] = 0;
		}

		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_FenceEvent == nullptr)
			THROW_COM_ERROR("Fence Event was nullptr");
	}

	void Direct3D12Context::CreatePipelineStateObjects()
	{
		HRESULT hr;

		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

		CD3DX12_DESCRIPTOR_RANGE descTableRanges[6] = {};
		descTableRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, ALBEDO_MAP_SHADER_REGISTER, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND); // Albedo Texture
		descTableRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, NORMAL_MAP_SHADER_REGISTER, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND); // Normal Texture
		descTableRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, ROUGHNESS_MAP_SHADER_REGISTER, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND); // Roughness Texture
		descTableRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, METALLIC_MAP_SHADER_REGISTER, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND); // Metallic Texture
		descTableRanges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, SPECULAR_MAP_SHADER_REGISTER, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND); // Specular Texture
		descTableRanges[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, AO_MAP_SHADER_REGISTER, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND); // AO Texture

		CD3DX12_ROOT_PARAMETER rootParams[8] = {};
		rootParams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); // ConstantBufferPerObject
		rootParams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); // Utilities
		rootParams[2].InitAsDescriptorTable(1, &descTableRanges[0], D3D12_SHADER_VISIBILITY_PIXEL); // Albedo Texture
		rootParams[3].InitAsDescriptorTable(1, &descTableRanges[1], D3D12_SHADER_VISIBILITY_PIXEL); // Normal Texture
		rootParams[4].InitAsDescriptorTable(1, &descTableRanges[2], D3D12_SHADER_VISIBILITY_PIXEL); // Roughness Texture
		rootParams[5].InitAsDescriptorTable(1, &descTableRanges[3], D3D12_SHADER_VISIBILITY_PIXEL); // Metallic Texture
		rootParams[6].InitAsDescriptorTable(1, &descTableRanges[4], D3D12_SHADER_VISIBILITY_PIXEL); // Specular Texture
		rootParams[7].InitAsDescriptorTable(1, &descTableRanges[5], D3D12_SHADER_VISIBILITY_PIXEL); // AO Texture

		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 0;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		// Create the root signature
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.Init(
			_countof(rootParams),
			rootParams,
			1,
			&sampler,
			rootSignatureFlags
		);

		ID3DBlob* RootSignatureByteCode = nullptr;
		ID3D10Blob* errorMsg = nullptr;
		hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &RootSignatureByteCode, &errorMsg);
		COM_ERROR_IF_FAILED(hr, "Failed to serialize Root Signature");

		hr = m_pLogicalDevice->CreateRootSignature(0, RootSignatureByteCode->GetBufferPointer(), RootSignatureByteCode->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature_ForwardPass));
		COM_ERROR_IF_FAILED(hr, "Failed to create Default Root Signature");

		// TODO: Make Shader class
		// TODO: Move this to the shader class
		// Compile vertex shader // TEMP//
		//LPCWSTR buildFolder = (LPCWSTR)_CRT_STRINGIZE(IE_BUILD_DIR);
		//LPCWSTR vertexShaderFolder = buildFolder + L"Shader_Vertex.cso";
		LPCWSTR vertexShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Shader.vertex.cso";
		LPCWSTR pixelShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Shader.pixel.cso";
		
		ComPtr<ID3DBlob> pErrorBuffer;
		ComPtr<ID3DBlob> pVertexShader;
		// Activate this for engine first launch to install shaders for client gpu
		/*hr = D3DCompileFromFile(L"Source/Shaders/ForwardRendering/Shader_Vertex.hlsl",
			nullptr,
			nullptr,
			"main",
			"vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&pVertexShader,
			&pErrorBuffer);*/
		hr = D3DReadFileToBlob(vertexShaderFolder, &pVertexShader);
		if (FAILED(hr))
		{
			IE_CORE_ERROR("Vertex Shader compilation error: {0}", (char*)pErrorBuffer->GetBufferPointer());
			COM_ERROR_IF_FAILED(hr, "Failed to compile Vertex Shader check log for more details.");
		}
		D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
		vertexShaderBytecode.BytecodeLength = pVertexShader->GetBufferSize();
		vertexShaderBytecode.pShaderBytecode = pVertexShader->GetBufferPointer();
		// Compile pixel shader // TEMP//
		// create vertex and pixel shaders
		ComPtr<ID3DBlob> pPixelShader = nullptr;
		// Activate this for engine first launch to install shaders for client gpu
		/*hr = D3DCompileFromFile(L"Source/Shaders/ForwardRendering/Shader_Pixel.hlsl",
			nullptr,
			nullptr,
			"main",
			"ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&pPixelShader,
			&pErrorBuffer);*/
		hr = D3DReadFileToBlob(pixelShaderFolder, &pPixelShader);
		if (FAILED(hr))
		{
			IE_CORE_ERROR("Pixel Shader compilation error: {0}", (char*)pErrorBuffer->GetBufferPointer());
			COM_ERROR_IF_FAILED(hr, "Failed to compile Pixel Shader check log for more details.");
		}
		D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
		pixelShaderBytecode.BytecodeLength = pPixelShader->GetBufferSize();
		pixelShaderBytecode.pShaderBytecode = pPixelShader->GetBufferPointer();

		// Create Input layout 
		D3D12_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0  },
			{ "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0  },
			{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0  },
		};

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
		inputLayoutDesc.pInputElementDescs = inputLayout;

		// Create Pipeline State Object
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = inputLayoutDesc;
		psoDesc.pRootSignature = m_pRootSignature_ForwardPass.Get();
		psoDesc.VS = vertexShaderBytecode;
		psoDesc.PS = pixelShaderBytecode;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc = m_SampleDesc;
		psoDesc.SampleMask = 0xffffffff;
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.DSVFormat = m_pDepthStencilBuffer->GetDesc().Format;
		psoDesc.NumRenderTargets = 1;

		hr = m_pLogicalDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pPipelineStateObject_ForwardPass));
		COM_ERROR_IF_FAILED(hr, "Failed to create default Pipeline State Object");
	}

	void Direct3D12Context::CreateConstantBufferResourceHeaps()
	{
		HRESULT hr;

		// PerObject Constant buffer
		for (int i = 0; i < m_FrameBufferCount; ++i)
		{
			hr = m_pLogicalDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_ConstantBufferUploadHeaps[i]));
			m_ConstantBufferUploadHeaps[i]->SetName(L"Constant Buffer Upload Resource Heap");

			CD3DX12_RANGE readRange(0, 0);
			hr = m_ConstantBufferUploadHeaps[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_cbvGPUAddress[i]));
		}

		// Per Frame
		for (int i = 0; i < m_FrameBufferCount; ++i)
		{
			hr = m_pLogicalDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_ConstantBufferPerFrameUploadHeaps[i]));
			if (FAILED(hr)) {
				IE_CORE_ERROR("Failed to create committed resource for Constant Buffer Per Frame data.")
			}
			m_ConstantBufferPerFrameUploadHeaps[i]->SetName(L"Constant Buffer PerFrame Upload Heap");
			CD3DX12_RANGE readRange(0, 0);
			hr = m_ConstantBufferPerFrameUploadHeaps[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_cbvPerFrameGPUAddress[i]));
		}

	}

	void Direct3D12Context::CreateViewport()
	{
		m_ViewPort.TopLeftX = 0;
		m_ViewPort.TopLeftY = 0;
		m_ViewPort.Width = static_cast<FLOAT>(m_WindowWidth);
		m_ViewPort.Height = static_cast<FLOAT>(m_WindowHeight);
		m_ViewPort.MinDepth = 0.0f;
		m_ViewPort.MaxDepth = 1.0f;
	}

	void Direct3D12Context::CreateScissorRect()
	{
		m_ScissorRect.left = 0;
		m_ScissorRect.top = 0;
		m_ScissorRect.right = m_WindowWidth;
		m_ScissorRect.bottom = m_WindowHeight;
	}

	void Direct3D12Context::CloseCommandListAndSignalCommandQueue()
	{
		m_pCommandList->Close();
		ID3D12CommandList* ppCommandLists[] = { m_pCommandList.Get() };
		m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		m_FenceValues[m_FrameIndex]++;
		HRESULT hr = m_pCommandQueue->Signal(m_pFences->GetAddressOf()[m_FrameIndex], m_FenceValues[m_FrameIndex]);
		COM_ERROR_IF_FAILED(hr, "Failed to signal command queue");
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

	void Direct3D12Context::LoadAssets()
	{
		LoadTextures();
	}

	void Direct3D12Context::LoadTextures()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE heapHandle(m_pMainDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		Texture::eTextureType albedo = Texture::eTextureType::ALBEDO;
		Texture::eTextureType normal = Texture::eTextureType::NORMAL;
		Texture::eTextureType secular = Texture::eTextureType::SPECULAR;

		//if (!texture.Init(L"../Assets/Objects/Dandelion/Textures/Atlas/Flower_Albedo.jpg", albedo, heapHandle))
		if (!albedoTexture.Init(L"../Assets/Models/nanosuit/body_dif.png", albedo, heapHandle))
			IE_CORE_ERROR("Failed to load texture in graphics context.");

		//if (!texture2.Init(L"../Assets/Objects/Dandelion/Textures/Atlas/Flower_Normal.jpg", normal, heapHandle))
		if (!normalTexture.Init(L"../Assets/Models/nanosuit/body_showroom_ddn.png", normal, heapHandle))
			IE_CORE_ERROR("Failed to load texture in graphics context.");
		
		if (!specularTexture.Init(L"../Assets/Models/nanosuit/body_showroom_spec.png", secular, heapHandle))
			IE_CORE_ERROR("Failed to load texture in graphics context.");

	}

	void Direct3D12Context::CreateCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		HRESULT hr = m_pLogicalDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCommandQueue));
		COM_ERROR_IF_FAILED(hr, "Failed to Create Command Queue");
	}

	void Direct3D12Context::CreateDXGIFactory()
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
		COM_ERROR_IF_FAILED(hr, "Failed to create DXGI Factory");

	}

	void Direct3D12Context::CreateDevice()
	{
		GetHardwareAdapter(m_pDxgiFactory.Get(), &m_pPhysicalDevice);

		HRESULT hr = D3D12CreateDevice(m_pPhysicalDevice.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pLogicalDevice));
		COM_ERROR_IF_FAILED(hr, "Failed to create logical device.");
	}

	void Direct3D12Context::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
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

	void Direct3D12Context::WaitForGPU()
	{
		// Schedule a Signal command in the queue.
		m_pCommandQueue->Signal(m_pFences[m_FrameIndex].Get(), m_FenceValues[m_FrameIndex]);

		// Wait until the fence has been processed.
		m_pFences[m_FrameIndex].Get()->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_FenceEvent);
		WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

		// Increment the fence value for the current frame.
		m_FenceValues[m_FrameIndex]++;
	}

	void Direct3D12Context::UpdateSizeDependentResources()
	{
		UpdateViewAndScissor();

		// Re-Create Render Target View
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			for (UINT i = 0; i < m_FrameBufferCount; i++)
			{
				m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargets[i]));
				m_pLogicalDevice->CreateRenderTargetView(m_pRenderTargets[i].Get(), nullptr, rtvHandle);
				rtvHandle.Offset(1, m_RtvDescriptorSize);
			}
		}

		// Re-Create Depth Stencil View
		{
			CreateDepthStencilBuffer();
		}

		// Recreate Camera Projection Matrix
		{
			camera.SetProjectionValues(camera.GetFOV(), static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight), camera.GetNearZ(), camera.GetFarZ());
		}

	}

	void Direct3D12Context::UpdateViewAndScissor()
	{
		float viewWidthRatio = static_cast<float>(m_ResolutionOptions[m_ResolutionIndex].Width) / m_WindowWidth;
		float viewHeighthRatio = static_cast<float>(m_ResolutionOptions[m_ResolutionIndex].Height) / m_WindowHeight;

		float x = 1.0f;
		float y = 1.0f;

		if (viewWidthRatio < viewHeighthRatio)
			x = viewWidthRatio / viewHeighthRatio;
		else
			y = viewHeighthRatio / viewWidthRatio;

		m_ViewPort.TopLeftX = m_WindowWidth * (1.0f - x) / 2.0f;
		m_ViewPort.TopLeftY = m_WindowHeight * (1.0f - y) / 2.0f;
		m_ViewPort.Width = x * m_WindowWidth;
		m_ViewPort.Height = y * m_WindowHeight;

		m_ScissorRect.left = static_cast<LONG>(m_ViewPort.TopLeftX);
		m_ScissorRect.right = static_cast<LONG>(m_ViewPort.TopLeftX + m_ViewPort.Width);
		m_ScissorRect.top = static_cast<LONG>(m_ViewPort.TopLeftY);
		m_ScissorRect.bottom = static_cast<LONG>(m_ViewPort.TopLeftX + m_ViewPort.Height);
	}

}