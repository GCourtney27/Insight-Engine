#include <ie_pch.h>

#include "Direct3D12_Context.h"

#include "Insight/Input/Input.h"
#include "Insight/Core/Application.h"
#include "Platform/Windows/Windows_Window.h"
#include "Insight/Runtime/APlayer_Character.h"



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
			CreateFenceEvent();
			CreateCommandAllocators();

			CreateConstantBuffers();
			CreateConstantBufferViews();

			PIXBeginEvent(m_pCommandQueue.Get(), 0, L"D3D12 Setup");
			{
				// Window
				{
					CreateSwapChain();
					CreateViewport();
					CreateScissorRect();
					CreateScreenQuad();
				}

				CreateRootSignature();
				CreateGeometryPassPSO();
				CreateLightPassPSO();
				CreateSkyPassPSO();
				CreatePostFxPassPSO();

				// Render Targets and Constant Buffers
				{
					CreateDSV();
					CreateRTVs();
					CreateRenderTargetViewDescriptorHeap();
				}

				LoadAssets();
			}
			PIXEndEvent(m_pCommandQueue.Get());

		}
		catch (COMException& ex) {
			COM_SAFE_RELEASE(m_pDxgiFactory);
			MessageBox(*m_pWindowHandle, ex.what(), L"Fatal Error", MB_OK);
			return false;
		}
		return true;
	}

	bool Direct3D12Context::PostInit()
	{
		CloseCommandListAndSignalCommandQueue();
		return true;
	}

	void Direct3D12Context::OnUpdate(const float& deltaTime)
	{
		ACamera& playerCamera = APlayerCharacter::Get().GetCameraRef();

		// Send Per-Frame Variables to GPU
		m_PerFrameData.cameraPosition = playerCamera.GetTransformRef().GetPosition();
		XMFLOAT4X4 inViewFloat;
		XMStoreFloat4x4(&inViewFloat, XMMatrixInverse(nullptr, playerCamera.GetViewMatrix()));
		XMFLOAT4X4 inProjFloat;
		XMStoreFloat4x4(&inProjFloat, XMMatrixInverse(nullptr, playerCamera.GetProjectionMatrix()));

		m_PerFrameData.invView = inViewFloat;
		m_PerFrameData.invProj = inProjFloat;
		m_PerFrameData.deltaMs = deltaTime;
		m_PerFrameData.time = (float)Application::Get().GetFrameTimer().seconds();
		m_PerFrameData.cameraNearZ = (float)playerCamera.GetNearZ();
		m_PerFrameData.cameraFarZ = (float)playerCamera.GetFarZ();
		m_PerFrameData.cameraExposure = (float)playerCamera.GetExposure();
		m_PerFrameData.numPointLights = (int)m_PointLights.size();
		m_PerFrameData.numDirectionalLights = (int)m_DirectionalLights.size();
		m_PerFrameData.numSpotLights = (int)m_SpotLights.size();
		memcpy(m_cbvPerFrameGPUAddress[m_FrameIndex], &m_PerFrameData, sizeof(CB_PS_VS_PerFrame));

		// Send Point Lights to GPU
		for (int i = 0; i < m_PointLights.size(); i++) {
			memcpy(m_cbvLightBufferGPUAddress[m_FrameIndex] + POINT_LIGHTS_CB_ALIGNED_OFFSET + (sizeof(CB_PS_PointLight) * i), &m_PointLights[i]->GetConstantBuffer(), sizeof(CB_PS_PointLight));
		}
		// Send Directionl Lights to GPU
		for (int i = 0; i < m_DirectionalLights.size(); i++) {
			memcpy(m_cbvLightBufferGPUAddress[m_FrameIndex] + DIRECTIONAL_LIGHTS_CB_ALIGNED_OFFSET + (sizeof(CB_PS_DirectionalLight) * i), &m_DirectionalLights[i]->GetConstantBuffer(), sizeof(CB_PS_DirectionalLight));
		}
		// Send Spot Lights to GPU
		for (int i = 0; i < m_SpotLights.size(); i++) {
			memcpy(m_cbvLightBufferGPUAddress[m_FrameIndex] + SPOT_LIGHTS_CB_ALIGNED_OFFSET + (sizeof(CB_PS_SpotLight) * i), &m_SpotLights[i]->GetConstantBuffer(), sizeof(CB_PS_SpotLight));
		}

		// Send Post-Fx data to GPU
		if (m_pPostFx) {
			memcpy(m_cbvPostFxGPUAddress[m_FrameIndex], &m_pPostFx->GetConstantBuffer(), sizeof(CB_PS_PostFx));
		}

	}

	void Direct3D12Context::MoveToNextFrame()
	{
		HRESULT hr;

		// Schedule a Signal command in the queue.
		const UINT64 currentFenceValue = m_FenceValues[m_FrameIndex];
		hr = m_pCommandQueue->Signal(m_pFence.Get(), currentFenceValue);
		ThrowIfFailed(hr, "Failed to signal fence on Command Queue");

		// Advance the frame index.
		m_FrameIndex = (m_FrameIndex + 1) % m_FrameBufferCount;

		// Check to see if the next frame is ready to start.
		if (m_pFence->GetCompletedValue() < m_FenceValues[m_FrameIndex])
		{
			hr = m_pFence->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_FenceEvent);
			ThrowIfFailed(hr, "Failed to set completion event on fence");
			WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
		}

		// Set the fence value for the next frame.
		m_FenceValues[m_FrameIndex] = currentFenceValue + 1;
	}

	void Direct3D12Context::OnPreFrameRender()
	{
		HRESULT hr;

		hr = m_pCommandAllocators[m_FrameIndex]->Reset();
		ThrowIfFailed(hr, "Failed to reset command allocator in Direct3D12Context::OnPreFrameRender");

		hr = m_pCommandList->Reset(m_pCommandAllocators[m_FrameIndex].Get(), m_pPipelineStateObject_GeometryPass.Get());
		ThrowIfFailed(hr, "Failed to reset command list in Direct3D12Context::OnPreFrameRender");

		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GetRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_pCommandList->ClearRenderTargetView(GetRenderTargetView(), clearColor, 0, nullptr);
		m_pCommandList->RSSetScissorRects(1, &m_ScissorRect);
		m_pCommandList->RSSetViewports(1, &m_ViewPort);

		BindGeometryPass();

	}

	void Direct3D12Context::BindGeometryPass(bool setPSO)
	{
		ID3D12DescriptorHeap* ppHeaps[] = { m_cbvsrvHeap.pDH.Get() };

		if (setPSO) {
			m_pCommandList->SetPipelineState(m_pPipelineStateObject_GeometryPass.Get());
		}

		for (int i = 0; i < m_NumRTV; i++)
			m_pCommandList->ClearRenderTargetView(m_rtvHeap.hCPU(i), m_ClearColor, 0, nullptr);

		m_pCommandList->ClearDepthStencilView(m_dsvHeap.hCPUHeapStart, D3D12_CLEAR_FLAG_DEPTH, m_ClearDepth, 0xff, 0, nullptr);

		m_pCommandList->OMSetRenderTargets(m_NumRTV, &m_rtvHeap.hCPUHeapStart, true, &m_dsvHeap.hCPUHeapStart);
		m_pCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		m_pCommandList->SetGraphicsRootSignature(m_pRootSignature.Get());
		m_pCommandList->SetGraphicsRootDescriptorTable(4, m_cbvsrvHeap.hGPU(0));

		{
			m_AlbedoTexture.Bind();
			m_NormalTexture.Bind();
			m_RoughnessTexture.Bind();
			m_MetallicTexture.Bind();
			m_AOTexture.Bind();

			m_Irradiance.Bind();
			m_Environment.Bind();
			m_BRDFLUT.Bind();
		}

	}

	void Direct3D12Context::OnMidFrameRender()
	{
		//m_pCommandList->OMSetRenderTargets(1, &GetRenderTargetView(), true, nullptr);
		m_pCommandList->OMSetRenderTargets(1, &m_rtvHeap.hCPU(4), true, nullptr);
		BindLightingPass();

		//m_pCommandList->OMSetRenderTargets(1, &GetRenderTargetView(), true, &m_dsvHeap.hCPUHeapStart);
		m_pCommandList->OMSetRenderTargets(1, &m_rtvHeap.hCPU(4), true, &m_dsvHeap.hCPUHeapStart);
		BindSkyPass();

		m_pCommandList->OMSetRenderTargets(1, &GetRenderTargetView(), true, nullptr);
		BindPostFxPass();
	}

	void Direct3D12Context::BindLightingPass()
	{
		for (unsigned int i = 0; i < m_NumRTV-1; ++i) {
			m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargetTextures[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
		}

		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));

		m_pCommandList->SetPipelineState(m_pPipelineStateObject_LightingPass.Get());

		m_ScreenQuad.Render(m_pCommandList);

	}

	void Direct3D12Context::BindSkyPass()
	{
		if (m_pSkySphere) {
			m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

			m_pCommandList->SetPipelineState(m_pPipelineStateObject_SkyPass.Get());

			m_pSkySphere->RenderSky(m_pCommandList);
		}
	}

	void Direct3D12Context::BindPostFxPass()
	{
		if (m_pPostFx) {
			m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));

			m_pCommandList->SetPipelineState(m_pPipelineStateObject_PostFxPass.Get());
			m_pCommandList->SetGraphicsRootDescriptorTable(14, m_cbvsrvHeap.hGPU(5));

			m_ScreenQuad.Render(m_pCommandList);
		}

	}

	void Direct3D12Context::PopulateCommandLists()
	{
		m_pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// Set Per-Frame Constant Buffers
		m_pCommandList->SetGraphicsRootConstantBufferView(1, m_PerFrameCBV[m_FrameIndex]->GetGPUVirtualAddress());
		// Set light buffer
		m_pCommandList->SetGraphicsRootConstantBufferView(2, m_LightCBV[m_FrameIndex]->GetGPUVirtualAddress());
		// Set Post-Fx buffer
		m_pCommandList->SetGraphicsRootConstantBufferView(3, m_PostFxCBV[m_FrameIndex]->GetGPUVirtualAddress());
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

		// Prepare the buffers for text frame
		{
			for (unsigned int i = 0; i < m_NumRTV-1; ++i) {
				m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargetTextures[i].Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
			}
			m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
		}

		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GetRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		hr = m_pCommandList->Close();
		ThrowIfFailed(hr, "Failed to close command list. Cannot execute draw commands");

		ID3D12CommandList* ppCommandLists[] = { m_pCommandList.Get() };
		m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		WaitForGPU();
	}

	void Direct3D12Context::SwapBuffers()
	{
		UINT presentFlags = (m_AllowTearing && m_WindowedMode) ? DXGI_PRESENT_ALLOW_TEARING : 0;
		HRESULT hr = m_pSwapChain->Present(m_VSyncEnabled, presentFlags);
		ThrowIfFailed(hr, "Failed to present frame");
		MoveToNextFrame();
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
					m_pRenderTargetTextures[i].Reset();
					m_pRenderTargets[i].Reset();
					m_pRenderTargetTextures_PostFxPass[i].Reset();
					m_FenceValues[i] = m_FenceValues[m_FrameIndex];
				}
				m_pDepthStencilTexture.Reset();

				DXGI_SWAP_CHAIN_DESC desc = {};
				m_pSwapChain->GetDesc(&desc);
				hr = m_pSwapChain->ResizeBuffers(m_FrameBufferCount, m_WindowWidth, m_WindowHeight, desc.BufferDesc.Format, desc.Flags);
				ThrowIfFailed(hr, "Failed to resize swap chain buffers");

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
					ThrowIfFailed(m_pSwapChain->GetContainingOutput(&pOutput), "Failed to get containing output");
					DXGI_OUTPUT_DESC Desc;
					ThrowIfFailed(pOutput->GetDesc(&Desc), "Failed to get description from output");
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
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.SampleDesc = m_SampleDesc;
		swapChainDesc.Flags = m_AllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain{};
		hr = m_pDxgiFactory->CreateSwapChainForHwnd(m_pCommandQueue.Get(), *m_pWindowHandle, &swapChainDesc, nullptr, nullptr, &swapChain);
		ThrowIfFailed(hr, "Failed to Create Swap Chain");

		if (m_AllowTearing)
		{
			hr = m_pDxgiFactory->MakeWindowAssociation(*m_pWindowHandle, DXGI_MWA_NO_ALT_ENTER);
			ThrowIfFailed(hr, "Failed to Make Window Association");
		}

		hr = swapChain.As(&m_pSwapChain);
		ThrowIfFailed(hr, "Failed to cast SwapChain ComPtr");

		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	}

	void Direct3D12Context::CreateRenderTargetViewDescriptorHeap()
	{
		HRESULT hr;
		WaitForGPU();

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = m_FrameBufferCount;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		hr = m_pLogicalDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_RTVDescriptorHeap));
		m_RTVDescriptorHeap->SetName(L"Render Target View Descriptor Heap");

		// All pending GPU work was already finished. Update the tracked fence values
		// to the last value signaled.
		for (UINT n = 0; n < m_FrameBufferCount; n++)
		{
			m_FenceValues[n] = m_FenceValues[m_FrameIndex];
		}

		m_FrameIndex = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE  hCPUHeapHandle = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_RTVDescriptorSize = m_pLogicalDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		for (UINT n = 0; n < m_FrameBufferCount; n++)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr = hCPUHeapHandle.ptr + m_RTVDescriptorSize * n;
			hr = m_pSwapChain->GetBuffer(n, IID_PPV_ARGS(&m_pRenderTargets[n]));
			ThrowIfFailed(hr, "Failed to get buffer in swap chain during descriptor heap initialization.");
			m_pLogicalDevice->CreateRenderTargetView(m_pRenderTargets[n].Get(), nullptr, handle);


			WCHAR name[25];
			swprintf_s(name, L"Render Target %d", n);
			m_pRenderTargets[n]->SetName(name);
		}
	}

	void Direct3D12Context::CreateDSV()
	{
		HRESULT hr;

		m_dsvHeap.Create(m_pLogicalDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
		CD3DX12_HEAP_PROPERTIES heapProperty(D3D12_HEAP_TYPE_DEFAULT);

		D3D12_RESOURCE_DESC resourceDesc;
		ZeroMemory(&resourceDesc, sizeof(resourceDesc));
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Alignment = 0;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = m_DsvFormat;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.Width = (UINT)m_WindowWidth;
		resourceDesc.Height = (UINT)m_WindowHeight;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE depthOptomizedClearValue = {};
		depthOptomizedClearValue = { m_DsvFormat , m_ClearDepth };

		hr = m_pLogicalDevice->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptomizedClearValue,
			IID_PPV_ARGS(&m_pDepthStencilTexture));
		m_pDepthStencilTexture->SetName(L"Depth Stencil Buffer");
		if (FAILED(hr))
			IE_CORE_ERROR("Failed to create comitted resource for depth stencil view");

		D3D12_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Texture2D.MipSlice = 0;
		desc.Format = resourceDesc.Format;
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		desc.Flags = D3D12_DSV_FLAG_NONE;

		m_pLogicalDevice->CreateDepthStencilView(m_pDepthStencilTexture.Get(), &desc, m_dsvHeap.hCPU(0));

		D3D12_SHADER_RESOURCE_VIEW_DESC dsvDesc = {};
		dsvDesc.Texture2D.MipLevels = resourceDesc.MipLevels;
		dsvDesc.Texture2D.MostDetailedMip = 0;
		dsvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		dsvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		dsvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		m_pLogicalDevice->CreateShaderResourceView(m_pDepthStencilTexture.Get(), &dsvDesc, m_cbvsrvHeap.hCPU(4));
	}

	void Direct3D12Context::CreateRTVs()
	{
		HRESULT hr;

		m_rtvHeap.Create(m_pLogicalDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 5);
		CD3DX12_HEAP_PROPERTIES heapProperty(D3D12_HEAP_TYPE_DEFAULT);

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Alignment = 0;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.MipLevels = 1;

		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.Width = (UINT)m_WindowWidth;
		resourceDesc.Height = (UINT)m_WindowHeight;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;


		D3D12_CLEAR_VALUE clearVal;
		clearVal.Color[0] = m_ClearColor[0];
		clearVal.Color[1] = m_ClearColor[1];
		clearVal.Color[2] = m_ClearColor[2];
		clearVal.Color[3] = m_ClearColor[3];

		for (int i = 0; i < m_NumRTV; i++) {
			resourceDesc.Format = m_RtvFormat[i];
			clearVal.Format = m_RtvFormat[i];
			hr = m_pLogicalDevice->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &clearVal, IID_PPV_ARGS(&m_pRenderTargetTextures[i]));
			ThrowIfFailed(hr, "Failed to create committed resource for RTV at index: " + std::to_string(i));
		}
		m_pRenderTargetTextures[0]->SetName(L"Render Target Texture Diffuse");
		m_pRenderTargetTextures[1]->SetName(L"Render Target Texture Normal");
		m_pRenderTargetTextures[2]->SetName(L"Render Target Texture (R)Roughness/(G)Metallic/(B)AO");
		m_pRenderTargetTextures[3]->SetName(L"Render Target Texture Position");
		m_pRenderTargetTextures[4]->SetName(L"Render Target Texture Light Pass Result");
		/*resourceDesc.Format = m_RtvFormat[4];
		clearVal.Format = m_RtvFormat[4];
		hr = m_pLogicalDevice->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &clearVal, IID_PPV_ARGS(&m_pRenderTargetTextures[4]));
		ThrowIfFailed(hr, "Failed to create committed resource for RTV at index: " + std::to_string(4));
		m_pRenderTargetTextures[4]->SetName(L"Render Target Texture Light Pass Result");*/


		D3D12_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Texture2D.MipSlice = 0;
		desc.Texture2D.PlaneSlice = 0;

		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		for (int i = 0; i < m_NumRTV; i++) {
			desc.Format = m_RtvFormat[i];
			m_pLogicalDevice->CreateRenderTargetView(m_pRenderTargetTextures[i].Get(), &desc, m_rtvHeap.hCPU(i));
		}
		/*desc.Format = m_RtvFormat[4];
		m_pLogicalDevice->CreateRenderTargetView(m_pRenderTargetTextures[4].Get(), &desc, m_rtvHeap.hCPU(4));*/

		//Create SRVs for Render Targets
		D3D12_SHADER_RESOURCE_VIEW_DESC descSRV = {};
		descSRV.Texture2D.MipLevels = resourceDesc.MipLevels;
		descSRV.Texture2D.MostDetailedMip = 0;
		descSRV.Format = resourceDesc.Format;
		descSRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		descSRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;


		for (int i = 0; i < m_NumRTV - 1; i++) {
			descSRV.Format = m_RtvFormat[i];
			m_pLogicalDevice->CreateShaderResourceView(m_pRenderTargetTextures[i].Get(), &descSRV, m_cbvsrvHeap.hCPU(i));
		}
		m_pRenderTargetTextures[0]->SetName(L"Render Target SRV Albedo");
		m_pRenderTargetTextures[1]->SetName(L"Render Target SRV Normal");
		m_pRenderTargetTextures[2]->SetName(L"Render Target SRV (R)Roughness/(G)Metallic/(B)AO");
		m_pRenderTargetTextures[3]->SetName(L"Render Target SRV Position");

		descSRV.Format = m_RtvFormat[4];
		m_pLogicalDevice->CreateShaderResourceView(m_pRenderTargetTextures[4].Get(), &descSRV, m_cbvsrvHeap.hCPU(5));
		m_pRenderTargetTextures[4]->SetName(L"Render Target SRV Light Pass Result");

	}

	void Direct3D12Context::CreateConstantBufferViews()
	{
		HRESULT hr = m_cbvsrvHeap.Create(m_pLogicalDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 25, true);
		ThrowIfFailed(hr, "Failed to create CBV SRV descriptor heap");
		
	}

	void Direct3D12Context::CreateRootSignature()
	{
		CD3DX12_DESCRIPTOR_RANGE range[11];
		range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 0); // G-Buffer inputs t0-t4

		range[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5); // PerObject texture inputs - Albedo
		range[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6); // PerObject texture inputs - Normal
		range[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 7); // PerObject texture inputs - Roughness
		range[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 8); // PerObject texture inputs - Metallic
		range[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 9); // PerObject texture inputs - AO

		range[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 10); // Sky - Irradiance
		range[7].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 11); // Sky - Environment Map
		range[8].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 12); // Sky - BRDF LUT
		range[9].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 13); // Sky - Diffuse

		range[10].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 14); // Post-FX Input

		CD3DX12_ROOT_PARAMETER rootParameters[15];
		rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);	  // Per-Object constant buffer
		rootParameters[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);		  // Per-Frame constant buffer
		rootParameters[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_PIXEL);	  // Light constant buffer
		rootParameters[3].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_PIXEL);	  // PostFx constant buffer
		rootParameters[4].InitAsDescriptorTable(1, &range[0], D3D12_SHADER_VISIBILITY_PIXEL); // G-Buffer inputs

		rootParameters[5].InitAsDescriptorTable(1, &range[1], D3D12_SHADER_VISIBILITY_PIXEL); // PerObject texture inputs - Albedo
		rootParameters[6].InitAsDescriptorTable(1, &range[2], D3D12_SHADER_VISIBILITY_PIXEL); // PerObject texture inputs - Normal
		rootParameters[7].InitAsDescriptorTable(1, &range[3], D3D12_SHADER_VISIBILITY_PIXEL); // PerObject texture inputs - Roughness
		rootParameters[8].InitAsDescriptorTable(1, &range[4], D3D12_SHADER_VISIBILITY_PIXEL); // PerObject texture inputs - Metallic
		rootParameters[9].InitAsDescriptorTable(1, &range[5], D3D12_SHADER_VISIBILITY_PIXEL); // PerObject texture inputs - AO

		rootParameters[10].InitAsDescriptorTable(1, &range[6], D3D12_SHADER_VISIBILITY_PIXEL); // Sky - Irradiance
		rootParameters[11].InitAsDescriptorTable(1, &range[7], D3D12_SHADER_VISIBILITY_PIXEL); // Sky - Environment Map
		rootParameters[12].InitAsDescriptorTable(1, &range[8], D3D12_SHADER_VISIBILITY_PIXEL); // Sky - BRDF LUT
		rootParameters[13].InitAsDescriptorTable(1, &range[9], D3D12_SHADER_VISIBILITY_PIXEL); // Sky - Diffuse

		rootParameters[14].InitAsDescriptorTable(1, &range[10], D3D12_SHADER_VISIBILITY_PIXEL); // Final Image




		CD3DX12_ROOT_SIGNATURE_DESC descRootSignature;
		descRootSignature.Init(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		CD3DX12_STATIC_SAMPLER_DESC staticSamplers[2];
		staticSamplers[0].Init(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
		staticSamplers[1].Init(1, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
		descRootSignature.NumStaticSamplers = _countof(staticSamplers);
		descRootSignature.pStaticSamplers = staticSamplers;

		ComPtr<ID3DBlob> rootSigBlob;
		ComPtr<ID3DBlob> errorBlob;

		HRESULT hr = D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, rootSigBlob.GetAddressOf(), errorBlob.GetAddressOf());
		ThrowIfFailed(hr, "Failed to serialize root signature");

		hr = m_pLogicalDevice->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature));
		ThrowIfFailed(hr, "Failed to create root signature");
	}

	void Direct3D12Context::CreateGeometryPassPSO()
	{
		HRESULT hr;

		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pPixelShader;

#if defined IE_DEBUG
		LPCWSTR vertexShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Geometry_Pass.vertex.cso";
		LPCWSTR pixelShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Geometry_Pass.pixel.cso";

#elif defined IE_RELEASE
		LPCWSTR vertexShaderFolder = L"Geometry_Pass.vertex.cso";
		LPCWSTR pixelShaderFolder = L"Geometry_Pass.pixel.cso";
#endif 

		hr = D3DReadFileToBlob(vertexShaderFolder, &pVertexShader);
		if (FAILED(hr)) {
			ThrowIfFailed(hr, "Failed to compile Vertex Shader check log for more details.");
		}
		hr = D3DReadFileToBlob(pixelShaderFolder, &pPixelShader);
		if (FAILED(hr)) {
			ThrowIfFailed(hr, "Failed to compile Pixel Shader check log for more details.");
		}


		D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
		vertexShaderBytecode.BytecodeLength = pVertexShader->GetBufferSize();
		vertexShaderBytecode.pShaderBytecode = pVertexShader->GetBufferPointer();

		D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
		pixelShaderBytecode.BytecodeLength = pPixelShader->GetBufferSize();
		pixelShaderBytecode.pShaderBytecode = pPixelShader->GetBufferPointer();

		D3D12_INPUT_ELEMENT_DESC inputLayout[5] =
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

		D3D12_GRAPHICS_PIPELINE_STATE_DESC descPipelineState = {};
		descPipelineState.VS = vertexShaderBytecode;
		descPipelineState.PS = pixelShaderBytecode;
		descPipelineState.InputLayout.pInputElementDescs = inputLayout;
		descPipelineState.InputLayout.NumElements = _countof(inputLayout);
		descPipelineState.pRootSignature = m_pRootSignature.Get();
		descPipelineState.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		descPipelineState.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		descPipelineState.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		descPipelineState.SampleMask = UINT_MAX;
		descPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		descPipelineState.NumRenderTargets = m_NumRTV;
		descPipelineState.RTVFormats[0] = m_RtvFormat[0];
		descPipelineState.RTVFormats[1] = m_RtvFormat[1];
		descPipelineState.RTVFormats[2] = m_RtvFormat[2];
		descPipelineState.RTVFormats[3] = m_RtvFormat[3];
		descPipelineState.DSVFormat = m_DsvFormat;
		descPipelineState.SampleDesc.Count = 1;

		hr = m_pLogicalDevice->CreateGraphicsPipelineState(&descPipelineState, IID_PPV_ARGS(&m_pPipelineStateObject_GeometryPass));
		ThrowIfFailed(hr, "Failed to create graphics pipeline state for geometry pass.");
		m_pPipelineStateObject_GeometryPass->SetName(L"PSO Geometry Pass");
	}

	void Direct3D12Context::CreateSkyPassPSO()
	{
		HRESULT hr;

		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pPixelShader;

#if defined IE_DEBUG
		LPCWSTR vertexShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Skybox.vertex.cso";
		LPCWSTR pixelShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Skybox.pixel.cso";
#elif defined IE_RELEASE
		LPCWSTR vertexShaderFolder = L"Skybox.vertex.cso";
		LPCWSTR pixelShaderFolder = L"Skybox.pixel.cso";
#endif 

		hr = D3DReadFileToBlob(vertexShaderFolder, &pVertexShader);
		if (FAILED(hr)) {
			ThrowIfFailed(hr, "Failed to compile Vertex Shader check log for more details.");
		}
		hr = D3DReadFileToBlob(pixelShaderFolder, &pPixelShader);
		if (FAILED(hr)) {
			ThrowIfFailed(hr, "Failed to compile Pixel Shader check log for more details.");
		}


		D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
		vertexShaderBytecode.BytecodeLength = pVertexShader->GetBufferSize();
		vertexShaderBytecode.pShaderBytecode = pVertexShader->GetBufferPointer();

		D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
		pixelShaderBytecode.BytecodeLength = pPixelShader->GetBufferSize();
		pixelShaderBytecode.pShaderBytecode = pPixelShader->GetBufferPointer();

		D3D12_INPUT_ELEMENT_DESC inputLayout[2] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
		inputLayoutDesc.pInputElementDescs = inputLayout;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
		auto depthStencilStateDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		depthStencilStateDesc.DepthEnable = true;
		depthStencilStateDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilStateDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		auto rasterizerStateDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		rasterizerStateDesc.DepthClipEnable = true;
		rasterizerStateDesc.CullMode = D3D12_CULL_MODE_FRONT;
		rasterizerStateDesc.FillMode = D3D12_FILL_MODE_SOLID;

		pipelineDesc.VS = vertexShaderBytecode;
		pipelineDesc.PS = pixelShaderBytecode;
		pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
		pipelineDesc.InputLayout.NumElements = _countof(inputLayout);
		pipelineDesc.pRootSignature = m_pRootSignature.Get();
		pipelineDesc.DepthStencilState = depthStencilStateDesc;
		pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pipelineDesc.RasterizerState = rasterizerStateDesc;
		pipelineDesc.SampleMask = UINT_MAX;
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineDesc.NumRenderTargets = 1;
		pipelineDesc.RTVFormats[0] = m_RtvFormat[0];
		pipelineDesc.SampleDesc.Count = 1;
		pipelineDesc.DSVFormat = m_DsvFormat;


		hr = m_pLogicalDevice->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&m_pPipelineStateObject_SkyPass));
		ThrowIfFailed(hr, "Failed to create skybox pipeline state object.");
		m_pPipelineStateObject_SkyPass->SetName(L"PSO Sky Pass");
	}

	void Direct3D12Context::CreateLightPassPSO()
	{
		HRESULT hr;

		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pPixelShader;
		//IE_BUILD_DIR
#if defined IE_DEBUG
		LPCWSTR vertexShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Light_Pass.vertex.cso";
		LPCWSTR pixelShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Light_Pass.pixel.cso";
#elif defined IE_RELEASE
		LPCWSTR vertexShaderFolder = L"Light_Pass.vertex.cso";
		LPCWSTR pixelShaderFolder = L"Light_Pass.pixel.cso";
#endif 

		hr = D3DReadFileToBlob(vertexShaderFolder, &pVertexShader);
		if (FAILED(hr)) {
			ThrowIfFailed(hr, "Failed to compile Vertex Shader check log for more details.");
		}
		hr = D3DReadFileToBlob(pixelShaderFolder, &pPixelShader);
		if (FAILED(hr)) {
			ThrowIfFailed(hr, "Failed to compile Pixel Shader check log for more details.");
		}


		D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
		vertexShaderBytecode.BytecodeLength = pVertexShader->GetBufferSize();
		vertexShaderBytecode.pShaderBytecode = pVertexShader->GetBufferPointer();

		D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
		pixelShaderBytecode.BytecodeLength = pPixelShader->GetBufferSize();
		pixelShaderBytecode.pShaderBytecode = pPixelShader->GetBufferPointer();

		D3D12_INPUT_ELEMENT_DESC inputLayout[2] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
		inputLayoutDesc.pInputElementDescs = inputLayout;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC descPipelineState = {};
		descPipelineState.VS = vertexShaderBytecode;
		descPipelineState.PS = pixelShaderBytecode;
		descPipelineState.InputLayout.pInputElementDescs = inputLayout;
		descPipelineState.InputLayout.NumElements = _countof(inputLayout);
		descPipelineState.pRootSignature = m_pRootSignature.Get();
		descPipelineState.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		descPipelineState.DepthStencilState.DepthEnable = false;
		descPipelineState.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		descPipelineState.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		descPipelineState.RasterizerState.DepthClipEnable = false;
		descPipelineState.SampleMask = UINT_MAX;
		descPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		descPipelineState.NumRenderTargets = 1;
		descPipelineState.RTVFormats[0] = m_RtvFormat[0];
		descPipelineState.SampleDesc.Count = 1;

		hr = m_pLogicalDevice->CreateGraphicsPipelineState(&descPipelineState, IID_PPV_ARGS(&m_pPipelineStateObject_LightingPass));
		ThrowIfFailed(hr, "Failed to create graphics pipeline state for lighting pass.");
		m_pPipelineStateObject_LightingPass->SetName(L"PSO Light Pass");
	}

	void Direct3D12Context::CreatePostFxPassPSO()
	{
		HRESULT hr;

		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pPixelShader;
		//IE_BUILD_DIR
#if defined IE_DEBUG
		LPCWSTR vertexShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/PostFx.vertex.cso";
		LPCWSTR pixelShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/PostFx.pixel.cso";
#elif defined IE_RELEASE
		LPCWSTR vertexShaderFolder = L"PostFx.vertex.cso";
		LPCWSTR pixelShaderFolder = L"PostFx.pixel.cso";
#endif 

		hr = D3DReadFileToBlob(vertexShaderFolder, &pVertexShader);
		if (FAILED(hr)) {
			ThrowIfFailed(hr, "Failed to compile Vertex Shader check log for more details.");
		}
		hr = D3DReadFileToBlob(pixelShaderFolder, &pPixelShader);
		if (FAILED(hr)) {
			ThrowIfFailed(hr, "Failed to compile Pixel Shader check log for more details.");
		}


		D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
		vertexShaderBytecode.BytecodeLength = pVertexShader->GetBufferSize();
		vertexShaderBytecode.pShaderBytecode = pVertexShader->GetBufferPointer();

		D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
		pixelShaderBytecode.BytecodeLength = pPixelShader->GetBufferSize();
		pixelShaderBytecode.pShaderBytecode = pPixelShader->GetBufferPointer();

		D3D12_INPUT_ELEMENT_DESC inputLayout[2] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
		inputLayoutDesc.pInputElementDescs = inputLayout;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC descPipelineState = {};
		descPipelineState.VS = vertexShaderBytecode;
		descPipelineState.PS = pixelShaderBytecode;
		descPipelineState.InputLayout.pInputElementDescs = inputLayout;
		descPipelineState.InputLayout.NumElements = _countof(inputLayout);
		descPipelineState.pRootSignature = m_pRootSignature.Get();
		descPipelineState.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		descPipelineState.DepthStencilState.DepthEnable = false;
		descPipelineState.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		descPipelineState.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		descPipelineState.RasterizerState.DepthClipEnable = false;
		descPipelineState.SampleMask = UINT_MAX;
		descPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		descPipelineState.NumRenderTargets = 1;
		descPipelineState.RTVFormats[0] = m_RtvFormat[0];
		descPipelineState.DSVFormat = m_DsvFormat;
		descPipelineState.SampleDesc.Count = 1;

		hr = m_pLogicalDevice->CreateGraphicsPipelineState(&descPipelineState, IID_PPV_ARGS(&m_pPipelineStateObject_PostFxPass));
		DWORD er = GetLastError();
		ThrowIfFailed(hr, "Failed to create graphics pipeline state for Post-Fx pass.");
		m_pPipelineStateObject_PostFxPass->SetName(L"PSO PostFx Pass");
	}

	void Direct3D12Context::CreateCommandAllocators()
	{
		HRESULT hr;

		for (int i = 0; i < m_FrameBufferCount; i++)
		{
			hr = m_pLogicalDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocators[i]));
			m_pCommandAllocators[i]->SetName(L"Graphics Command Allocator");
			ThrowIfFailed(hr, "Failed to Create Command Allocator");
		}

		hr = m_pLogicalDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pCommandList));
		m_pCommandList->SetName(L"Graphics Command List");
		ThrowIfFailed(hr, "Failed to Create Command List");
	}

	void Direct3D12Context::CreateFenceEvent()
	{
		HRESULT hr;
		hr = m_pLogicalDevice->CreateFence(m_FenceValues[m_FrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
		ThrowIfFailed(hr, "Failed to create Fence Event");
		m_FenceValues[m_FrameIndex]++;


		m_FenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		if (m_FenceEvent == nullptr)
			THROW_COM_ERROR("Fence Event was nullptr");
	}

	void Direct3D12Context::LoadAssets()
	{
		// TODO Load these per object
		// Textures
		std::wstring texRelPathAlbedoW = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath("Textures/RustedIron/RustedIron_Albedo.png"));
		Texture::eTextureType texTypeAlbedo = Texture::eTextureType::ALBEDO;
		m_AlbedoTexture.Init(texRelPathAlbedoW, texTypeAlbedo, m_cbvsrvHeap);

		std::wstring texRelPathNormalW = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath("Textures/RustedIron/RustedIron_Normal.png"));
		Texture::eTextureType texTypeNormal = Texture::eTextureType::NORMAL;
		m_NormalTexture.Init(texRelPathNormalW, texTypeNormal, m_cbvsrvHeap);

		std::wstring texRelPathRoughnessW = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath("Textures/RustedIron/RustedIron_Roughness.png"));
		Texture::eTextureType texTypeSpecular = Texture::eTextureType::ROUGHNESS;
		m_RoughnessTexture.Init(texRelPathRoughnessW, texTypeSpecular, m_cbvsrvHeap);

		std::wstring texRelPathMetallicW = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath("Textures/RustedIron/RustedIron_Metallic.png"));
		Texture::eTextureType texTypeMetallic = Texture::eTextureType::METALLIC;
		m_MetallicTexture.Init(texRelPathMetallicW, texTypeMetallic, m_cbvsrvHeap);

		std::wstring texRelPathAOW = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath("Textures/RustedIron/RustedIron_AO.png"));
		Texture::eTextureType texTypeAO = Texture::eTextureType::AO;
		m_AOTexture.Init(texRelPathAOW, texTypeAO, m_cbvsrvHeap);

		// skybox1
		// skybox2
		// skybox3
		// MountainTop
		// NewportLoft
		std::wstring texRelPathIRW = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath("Textures/Skyboxes/MountainTop_IR.dds"));
		Texture::eTextureType texTypeIR = Texture::eTextureType::SKY_IRRADIENCE;
		m_Irradiance.Init(texRelPathIRW, texTypeIR, m_cbvsrvHeap);

		std::wstring texRelPathEnvW = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath("Textures/Skyboxes/MountainTop_EnvMap.dds"));
		Texture::eTextureType texTypeEnv = Texture::eTextureType::SKY_ENVIRONMENT_MAP;
		m_Environment.Init(texRelPathEnvW, texTypeEnv, m_cbvsrvHeap);

		std::wstring texRelPathBRDFLUTW = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath("Textures/Skyboxes/ibl_brdf_lut.png"));
		Texture::eTextureType texTypeBRDFLUT = Texture::eTextureType::SKY_BRDF_LUT;
		m_BRDFLUT.Init(texRelPathBRDFLUTW, texTypeBRDFLUT, m_cbvsrvHeap);

	}

	void Direct3D12Context::CreateConstantBuffers()
	{
		HRESULT hr;

		// Light Constant buffer
		for (int i = 0; i < m_FrameBufferCount; ++i)
		{
			hr = m_pLogicalDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_LightCBV[i]));
			m_LightCBV[i]->SetName(L"Constant Buffer Light Buffer Upload Resource Heap");
			ThrowIfFailed(hr, "Failed to create upload heap for light buffer upload resource heaps");
			CD3DX12_RANGE readRange(0, 0);
			hr = m_LightCBV[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_cbvLightBufferGPUAddress[i]));
			ThrowIfFailed(hr, "Failed to map upload heap for light buffer upload resource heaps");
		}

		// PerObject Constant buffer
		for (int i = 0; i < m_FrameBufferCount; ++i)
		{
			hr = m_pLogicalDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_PerObjectCBV[i]));
			m_PerObjectCBV[i]->SetName(L"Constant Buffer Per-Object Upload Resource Heap");
			ThrowIfFailed(hr, "Failed to create upload heap for per-object upload resource heaps");
			CD3DX12_RANGE readRange(0, 0);
			hr = m_PerObjectCBV[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_cbvPerObjectGPUAddress[i]));
			ThrowIfFailed(hr, "Failed to map upload heap for per-object upload resource heaps");
		}

		// Per Frame
		for (int i = 0; i < m_FrameBufferCount; ++i) {
			hr = m_pLogicalDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_PerFrameCBV[i]));
			m_PerFrameCBV[i]->SetName(L"Constant Buffer Per-Frame Upload Heap");
			ThrowIfFailed(hr, "Failed to create upload heap for per-frame upload resource heaps");
			CD3DX12_RANGE readRange(0, 0);
			hr = m_PerFrameCBV[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_cbvPerFrameGPUAddress[i]));
			ThrowIfFailed(hr, "Failed to create map heap for per-frame upload resource heaps");
		}

		// Post-Fx
		for (int i = 0; i < m_FrameBufferCount; ++i) {
			hr = m_pLogicalDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_PostFxCBV[i]));
			m_PostFxCBV[i]->SetName(L"Constant Buffer Per-Frame Upload Heap");
			ThrowIfFailed(hr, "Failed to create upload heap for per-frame upload resource heaps");
			CD3DX12_RANGE readRange(0, 0);
			hr = m_PostFxCBV[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_cbvPostFxGPUAddress[i]));
			ThrowIfFailed(hr, "Failed to create map heap for per-frame upload resource heaps");
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

	void Direct3D12Context::CreateScreenQuad()
	{
		m_ScreenQuad.Init();
	}

	void Direct3D12Context::CloseCommandListAndSignalCommandQueue()
	{
		m_pCommandList->Close();
		ID3D12CommandList* ppCommandLists[] = { m_pCommandList.Get() };
		m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		m_FenceValues[m_FrameIndex]++;
		HRESULT hr = m_pCommandQueue->Signal(m_pFence.Get(), m_FenceValues[m_FrameIndex]);
		if (FAILED(hr)) {
			IE_CORE_WARN("Command queue failed to signal.");
		}

		WaitForGPU();
	}

	void Direct3D12Context::CreateCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		HRESULT hr = m_pLogicalDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCommandQueue));
		ThrowIfFailed(hr, "Failed to Create Command Queue");
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
		ThrowIfFailed(hr, "Failed to create DXGI Factory.");

	}

	void Direct3D12Context::CreateDevice()
	{
		GetHardwareAdapter(m_pDxgiFactory.Get(), &m_pPhysicalDevice);

		HRESULT hr = D3D12CreateDevice(m_pPhysicalDevice.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pLogicalDevice));
		ThrowIfFailed(hr, "Failed to create logical device.");
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

				//OutputDebugStringW(desc.Description);
				//IE_CORE_INFO("Found suitable graphics hardware: {0}", std::string((char*)desc.Description));

			}
		}
	}

	void Direct3D12Context::WaitForGPU()
	{
		HRESULT hr;
		// Schedule a Signal command in the queue.
		hr = m_pCommandQueue->Signal(m_pFence.Get(), m_FenceValues[m_FrameIndex]);
		ThrowIfFailed(hr, "Fialed to signal command queue while waiting for GPU");

		// Wait until the fence has been processed.
		hr = m_pFence.Get()->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_FenceEvent);
		ThrowIfFailed(hr, "Fialed to set fence on completion event while waiting for GPU");

		WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

		// Increment the fence value for the current frame.
		m_FenceValues[m_FrameIndex]++;
	}

	void Direct3D12Context::UpdateSizeDependentResources()
	{
		UpdateViewAndScissor();

		// Re-Create Render Target View
		{
			for (UINT i = 0; i < m_FrameBufferCount; i++)
			{
				m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargets[i]));
				m_pLogicalDevice->CreateRenderTargetView(m_pRenderTargets[i].Get(), nullptr, m_rtvHeap.hCPU(i));
			}
		}

		// Re-Create Depth Stencil View
		{
			CreateDSV();
			CreateRTVs();
			m_RTVDescriptorHeap.Reset();
			CreateRenderTargetViewDescriptorHeap();
		}

		// Recreate Camera Projection Matrix
		{
			ACamera& camera = APlayerCharacter::Get().GetCameraRef();
			if (!camera.GetIsOrthographic()) {
				camera.SetPerspectiveProjectionValues(camera.GetFOV(), static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight), camera.GetNearZ(), camera.GetFarZ());
			}
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

	void ScreenQuad::Init()
	{
		HRESULT hr;

		ScreenSpaceVertex quadVerts[] =
		{
			{ { -1.0f, 1.0f, 0.0f }, { 0.0f,0.0f } }, // Top Left
			{ {  1.0f, 1.0f, 0.0f }, { 1.0f,0.0f } }, // Top Right
			{ { -1.0f,-1.0f, 0.0f }, { 0.0f,1.0f } }, // Bottom Left
			{ {  1.0f,-1.0f, 0.0f }, { 1.0f,1.0f } }, // Bottom Right
		};
		int vBufferSize = sizeof(quadVerts);
		m_NumVerticies = vBufferSize / sizeof(ScreenSpaceVertex);

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.Width = vBufferSize;
		resourceDesc.Height = 1;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		hr = Direct3D12Context::Get().GetDeviceContext().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_VertexBuffer)
		);
		m_VertexBuffer->SetName(L"Screen Quad Default Resource Heap");
		ThrowIfFailed(hr, "Failed to create default heap resource for screen qauad");

		ID3D12Resource* vBufferUploadHeap;
		hr = Direct3D12Context::Get().GetDeviceContext().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vBufferUploadHeap));
		vBufferUploadHeap->SetName(L"Screen Quad Upload Resource Heap");
		ThrowIfFailed(hr, "Failed to create upload heap resource for screen qauad");

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<BYTE*>(quadVerts);
		vertexData.RowPitch = vBufferSize;
		vertexData.SlicePitch = vBufferSize;

		UpdateSubresources(&Direct3D12Context::Get().GetCommandList(), m_VertexBuffer.Get(), vBufferUploadHeap, 0, 0, 1, &vertexData);

		Direct3D12Context::Get().GetCommandList().ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_VertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = sizeof(ScreenSpaceVertex);
		m_VertexBufferView.SizeInBytes = vBufferSize;

	}

	void ScreenQuad::Render(ComPtr<ID3D12GraphicsCommandList> commandList)
	{
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		commandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
		commandList->DrawInstanced(m_NumVerticies, 1, 0, 0);
	}


}