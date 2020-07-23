#include <ie_pch.h>

#include "Direct3D12_Context.h"

#include "Insight/Core/Application.h"
#include "Platform/Windows/Windows_Window.h"
#include "Insight/Runtime/APlayer_Character.h"
#include "Insight/Systems/Managers/Geometry_Manager.h"

#include "Insight/Rendering/APost_Fx.h"
#include "Insight/Rendering/ASky_Light.h"
#include "Insight/Rendering/ASky_Sphere.h"
#include "Insight/Rendering/Lighting/ASpot_Light.h"
#include "Insight/Rendering/Lighting/APoint_Light.h"
#include "Insight/Rendering/Lighting/ADirectional_Light.h"

#include "Platform/Windows/DirectX_12/Geometry/D3D12_Vertex_Buffer.h"
#include "Platform/Windows/DirectX_12/Geometry/D3D12_Index_Buffer.h"
#include "Platform/Windows/DirectX_12/Geometry/Sphere_Renderer.h"

namespace Insight {


	Direct3D12Context::Direct3D12Context(WindowsWindow* WindowHandle)
		: m_pWindowHandle(&WindowHandle->GetWindowHandleReference()),
		m_pWindow(WindowHandle),
		Renderer(WindowHandle->GetWidth(), WindowHandle->GetHeight(), false)
	{
		IE_CORE_ASSERT(WindowHandle, "Window handle is NULL!");
		m_AspectRatio = static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight);
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

		if (!m_AllowTearing) {
			m_pSwapChain->SetFullscreenState(false, NULL);
		}

		if (!CloseHandle(m_FenceEvent)) {
			IE_CORE_ERROR("Failed to close GPU handle while cleaning up the renderer.");
		}
	}

	bool Direct3D12Context::InitImpl()
	{
		IE_CORE_INFO("Renderer: D3D 12");

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
				// Window adn Viewport
				{
					CreateSwapChain();
					CreateViewport();
					CreateScissorRect();
					CreateScreenQuad();
				}

				// Load Pipelines
				{
					CreateRootSignature();
					CreateShadowPassPSO();
					CreateGeometryPassPSO();
					CreateLightPassPSO();
					CreateSkyPassPSO();
					CreatePostFxPassPSO();
				}

				// Render Targets and Constant Buffers
				{
					CreateDSVs();
					CreateRTVs();
					CreateRenderTargetViewDescriptorHeap();
				}

				// Load Test Assets
				LoadDemoAssets();
			}
			PIXEndEvent(m_pCommandQueue.Get());
		}
		catch (COMException& ex) {
			m_pWindow->CreateMessageBox(ex.what(), L"Fatal Error");
			return false;
		}
		return true;
	}

	void Direct3D12Context::DestroyImpl()
	{
		Cleanup();
	}

	bool Direct3D12Context::PostInitImpl()
	{
		CloseCommandListAndSignalCommandQueue();
		m_pWorldCamera = &ACamera::Get();

		return true;
	}

	void Direct3D12Context::OnUpdateImpl(const float DeltaMs)
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		// Send Per-Frame Data to GPU
		XMFLOAT4X4 viewFloat;
		XMStoreFloat4x4(&viewFloat, XMMatrixTranspose(m_pWorldCamera->GetViewMatrix()));
		XMFLOAT4X4 projectionFloat;
		XMStoreFloat4x4(&projectionFloat, XMMatrixTranspose(m_pWorldCamera->GetProjectionMatrix()));
		m_PerFrameData.view = viewFloat;
		m_PerFrameData.projection = projectionFloat;
		m_PerFrameData.cameraPosition = m_pWorldCamera->GetTransformRef().GetPosition();
		m_PerFrameData.deltaMs = DeltaMs;
		m_PerFrameData.time = (float)Application::Get().GetFrameTimer().Seconds();
		m_PerFrameData.cameraNearZ = (float)m_pWorldCamera->GetNearZ();
		m_PerFrameData.cameraFarZ = (float)m_pWorldCamera->GetFarZ();
		m_PerFrameData.cameraExposure = (float)m_pWorldCamera->GetExposure();
		m_PerFrameData.numPointLights = (float)m_PointLights.size();
		m_PerFrameData.numDirectionalLights = (float)m_DirectionalLights.size();
		m_PerFrameData.numSpotLights = (float)m_SpotLights.size();
		m_PerFrameData.screenSize.x = (float)m_WindowWidth;
		m_PerFrameData.screenSize.y = (float)m_WindowHeight;
		memcpy(m_cbvPerFrameGPUAddress, &m_PerFrameData, sizeof(CB_PS_VS_PerFrame));

		// Send Point Lights to GPU
		for (int i = 0; i < m_PointLights.size(); i++) {
			memcpy(m_cbvLightBufferGPUAddress + POINT_LIGHTS_CB_ALIGNED_OFFSET + (sizeof(CB_PS_PointLight) * i), &m_PointLights[i]->GetConstantBuffer(), sizeof(CB_PS_PointLight));
		}
		// Send Directionl Lights to GPU
		for (int i = 0; i < m_DirectionalLights.size(); i++) {
			memcpy(m_cbvLightBufferGPUAddress + DIRECTIONAL_LIGHTS_CB_ALIGNED_OFFSET + (sizeof(CB_PS_DirectionalLight) * i), &m_DirectionalLights[i]->GetConstantBuffer(), sizeof(CB_PS_DirectionalLight));
		}
		// Send Spot Lights to GPU
		for (int i = 0; i < m_SpotLights.size(); i++) {
			memcpy(m_cbvLightBufferGPUAddress + SPOT_LIGHTS_CB_ALIGNED_OFFSET + (sizeof(CB_PS_SpotLight) * i), &m_SpotLights[i]->GetConstantBuffer(), sizeof(CB_PS_SpotLight));
		}

		// Send Post-Fx data to GPU
		if (m_pPostFx) {
			memcpy(m_cbvPostFxGPUAddress, &m_pPostFx->GetConstantBuffer(), sizeof(CB_PS_PostFx));
		}
	}

	void Direct3D12Context::OnPreFrameRenderImpl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		// Reset Command Allocators
		ThrowIfFailed(m_pScenePassCommandAllocators[m_FrameIndex]->Reset(),
			"Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Scene Pass");

		ThrowIfFailed(m_pShadowPassCommandAllocators[m_FrameIndex]->Reset(),
			"Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Shadow Pass");

		// Reset Command Lists
		ThrowIfFailed(m_pScenePassCommandList->Reset(m_pScenePassCommandAllocators[m_FrameIndex].Get(), m_pPipelineStateObject_GeometryPass.Get()),
			"Failed to reset command list in Direct3D12Context::OnPreFrameRender for Scene Pass");

		ThrowIfFailed(m_pShadowPassCommandList->Reset(m_pShadowPassCommandAllocators[m_FrameIndex].Get(), m_pPipelineStateObject_ShadowPass.Get()),
			"Failed to reset command list in Direct3D12Context::OnPreFrameRender for Shadow Pass");

		m_pScenePassCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GetRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// Reset Scene Pass
		static float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_pScenePassCommandList->ClearRenderTargetView(GetRenderTargetView(), ClearColor, 0, nullptr);
		m_pScenePassCommandList->RSSetScissorRects(1, &m_ScenePassScissorRect);
		m_pScenePassCommandList->RSSetViewports(1, &m_ScenePassViewPort);

		// Reset Shadow Pass
		m_pShadowPassCommandList->RSSetScissorRects(1, &m_ShadowPassScissorRect);
		m_pShadowPassCommandList->RSSetViewports(1, &m_ShadowPassViewPort);
		m_pShadowPassCommandList->OMSetRenderTargets(0, nullptr, FALSE, &m_dsvHeap.hCPU(1));
	}

	void Direct3D12Context::OnRenderImpl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		// Render Shadows
		m_pActiveCommandList = m_pShadowPassCommandList;
		BindShadowPass(); 
						
		// Render Scene
		m_pActiveCommandList = m_pScenePassCommandList;
		BindGeometryPass();
	}

	void Direct3D12Context::BindShadowPass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		ID3D12DescriptorHeap* ppHeaps[] = { m_cbvsrvHeap.pDH.Get() };
		m_pShadowPassCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		m_pShadowPassCommandList->SetPipelineState(m_pPipelineStateObject_ShadowPass.Get());
		m_pShadowPassCommandList->SetGraphicsRootSignature(m_pRootSignature.Get());
		m_pShadowPassCommandList->ClearDepthStencilView(m_dsvHeap.hCPU(1), D3D12_CLEAR_FLAG_DEPTH, m_DepthClearValue, 0xff, 0, nullptr);

		m_pShadowPassCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pShadowPassCommandList->SetGraphicsRootConstantBufferView(1, m_PerFrameCBV->GetGPUVirtualAddress());
		m_pShadowPassCommandList->SetGraphicsRootConstantBufferView(2, m_LightCBV->GetGPUVirtualAddress());

		// TODO Shadow pass logic here put this on another thread
		GeometryManager::Render(eRenderPass::RenderPass_Shadow);
	}

	void Direct3D12Context::BindGeometryPass(bool setPSO)
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		ID3D12DescriptorHeap* ppHeaps[] = { m_cbvsrvHeap.pDH.Get() };
		m_pScenePassCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		if (setPSO) {
			m_pScenePassCommandList->SetPipelineState(m_pPipelineStateObject_GeometryPass.Get());
		}

		for (int i = 0; i < m_NumRTV - 1; i++) {
			m_pScenePassCommandList->ClearRenderTargetView(m_rtvHeap.hCPU(i), m_ClearColor, 0, nullptr);
		}

		m_pScenePassCommandList->ClearDepthStencilView(m_dsvHeap.hCPU(0), D3D12_CLEAR_FLAG_DEPTH, m_DepthClearValue, 0xff, 0, nullptr);

		m_pScenePassCommandList->OMSetRenderTargets(m_NumRTV, &m_rtvHeap.hCPUHeapStart, true, &m_dsvHeap.hCPU(0));
		m_pScenePassCommandList->SetGraphicsRootSignature(m_pRootSignature.Get());
		// Set Scene Depth Texture
		m_pScenePassCommandList->SetGraphicsRootDescriptorTable(5, m_cbvsrvHeap.hGPU(0));
		// Set Shadow Depth Texture
		m_pScenePassCommandList->SetGraphicsRootDescriptorTable(11, m_cbvsrvHeap.hGPU(6));

		{
			m_pScenePassCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_pScenePassCommandList->SetGraphicsRootConstantBufferView(3, m_PostFxCBV->GetGPUVirtualAddress());
			m_pScenePassCommandList->SetGraphicsRootConstantBufferView(2, m_LightCBV->GetGPUVirtualAddress());
			m_pScenePassCommandList->SetGraphicsRootConstantBufferView(1, m_PerFrameCBV->GetGPUVirtualAddress());
		}

		GeometryManager::Render(eRenderPass::RenderPass_Scene);
	}

	void Direct3D12Context::OnMidFrameRenderImpl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE

		m_pScenePassCommandList->OMSetRenderTargets(1, &m_rtvHeap.hCPU(4), true, nullptr);
		BindLightingPass();

		m_pScenePassCommandList->OMSetRenderTargets(1, &m_rtvHeap.hCPU(4), true, &m_dsvHeap.hCPU(0));
		BindSkyPass();

		m_pScenePassCommandList->OMSetRenderTargets(1, &GetRenderTargetView(), true, nullptr);
		BindPostFxPass();
	}

	void Direct3D12Context::BindLightingPass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		if (m_SkyLight) {
			m_SkyLight->OnRender();
		}

		for (unsigned int i = 0; i < m_NumRTV - 1; ++i) {
			m_pScenePassCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargetTextures[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
		}

		m_pScenePassCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));

		m_pScenePassCommandList->SetPipelineState(m_pPipelineStateObject_LightingPass.Get());

		m_ScreenQuad.OnRender(m_pScenePassCommandList);

	}

	void Direct3D12Context::BindSkyPass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		if (m_pSkySphere) {

			m_pScenePassCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

			m_pScenePassCommandList->SetPipelineState(m_pPipelineStateObject_SkyPass.Get());

			m_pSkySphere->RenderSky(m_pScenePassCommandList);
		}
	}

	void Direct3D12Context::BindPostFxPass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		if (m_pPostFx) {

			m_pScenePassCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));

			m_pScenePassCommandList->SetPipelineState(m_pPipelineStateObject_PostFxPass.Get());
			m_pScenePassCommandList->SetGraphicsRootDescriptorTable(16, m_cbvsrvHeap.hGPU(5));

			m_ScreenQuad.OnRender(m_pScenePassCommandList);
		}
	}

	void Direct3D12Context::ExecuteDrawImpl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		// Prepare the buffers for next frame
		{
			for (unsigned int i = 0; i < m_NumRTV - 1; ++i) {
				m_pScenePassCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargetTextures[i].Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
			}
			m_pScenePassCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
		}

		// Prepare render target to be presented
		m_pScenePassCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GetRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		ThrowIfFailed(m_pScenePassCommandList->Close(), "Failed to close command list for scene pass.");
		ThrowIfFailed(m_pShadowPassCommandList->Close(), "Failed to close the command list for shadow pass.");

		ID3D12CommandList* ppCommandLists[] = {
			m_pShadowPassCommandList.Get(), // Execure shadow pass first because we'll need the depth textures for the light pass
			m_pScenePassCommandList.Get()
		};
		m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		WaitForGPU();
	}

	void Direct3D12Context::SwapBuffersImpl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		UINT PresentFlags = (m_AllowTearing && m_WindowedMode) ? DXGI_PRESENT_ALLOW_TEARING : 0;
		HRESULT hr = m_pSwapChain->Present(m_VSyncEnabled, PresentFlags);
		ThrowIfFailed(hr, "Failed to present frame");
		MoveToNextFrame();
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

	void Direct3D12Context::OnWindowResizeImpl()
	{
		if (!m_IsMinimized) {

			if (m_WindowResizeComplete) {

				m_WindowResizeComplete = false;

				WaitForGPU();
				HRESULT hr;

				for (UINT i = 0; i < m_FrameBufferCount; i++) {
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

	void Direct3D12Context::OnWindowFullScreenImpl()
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

	void Direct3D12Context::SetVertexBuffersImpl(uint32_t StartSlot, uint32_t NumBuffers, ieVertexBuffer* pBuffers)
	{
		m_pActiveCommandList->IASetVertexBuffers(StartSlot, NumBuffers, reinterpret_cast<D3D12VertexBuffer*>(pBuffers)->GetVertexBufferView());
	}

	void Direct3D12Context::SetIndexBufferImpl(ieIndexBuffer* pBuffer)
	{
		m_pActiveCommandList->IASetIndexBuffer(&reinterpret_cast<D3D12IndexBuffer*>(pBuffer)->GetIndexBufferView());
	}

	void Direct3D12Context::DrawIndexedInstancedImpl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation)
	{
		m_pActiveCommandList->DrawIndexedInstanced(IndexCountPerInstance, NumInstances, StartIndexLocation, BaseVertexLoaction, StartInstanceLocation);
	}

	void Direct3D12Context::RenderSkySphereImpl()
	{
		m_SkySphere->Render(m_pScenePassCommandList);
	}

	bool Direct3D12Context::CreateSkyboxImpl()
	{
		m_SkySphere = new Sphere();
		m_SkySphere->Init(10, 20, 20);

		return true;
	}

	void Direct3D12Context::DestroySkyboxImpl()
	{
		if (m_SkySphere) {
			delete m_pSkySphere;
		}
	}

	void Direct3D12Context::CreateSwapChain()
	{
		HRESULT hr;

		DXGI_MODE_DESC BackBufferDesc = {};
		BackBufferDesc.Width = m_WindowWidth;
		BackBufferDesc.Height = m_WindowHeight;

		m_SampleDesc = {};
		m_SampleDesc.Count = 1;

		DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
		SwapChainDesc.BufferCount = m_FrameBufferCount;
		SwapChainDesc.Width = m_WindowWidth;
		SwapChainDesc.Height = m_WindowHeight;
		SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		SwapChainDesc.SampleDesc = m_SampleDesc;
		SwapChainDesc.Flags = m_AllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> SwapChain{};
		hr = m_pDxgiFactory->CreateSwapChainForHwnd(m_pCommandQueue.Get(), *m_pWindowHandle, &SwapChainDesc, nullptr, nullptr, &SwapChain);
		ThrowIfFailed(hr, "Failed to Create Swap Chain for D3D 12 context.");

		if (m_AllowTearing)
		{
			hr = m_pDxgiFactory->MakeWindowAssociation(*m_pWindowHandle, DXGI_MWA_NO_ALT_ENTER);
			ThrowIfFailed(hr, "Failed to make window association for D3D 12 context.");
		}

		hr = SwapChain.As(&m_pSwapChain);
		ThrowIfFailed(hr, "Failed to cast SwapChain ComPtr for D3D 12 context.");

		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	}

	void Direct3D12Context::CreateRenderTargetViewDescriptorHeap()
	{
		HRESULT hr;
		WaitForGPU();

		D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
		HeapDesc.NumDescriptors = m_FrameBufferCount;
		HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		hr = m_pDeviceContext->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&m_RTVDescriptorHeap));
		m_RTVDescriptorHeap->SetName(L"Render Target View Descriptor Heap");

		// All pending GPU work was already finished. Update the tracked fence values
		// to the last value signaled.
		for (UINT n = 0; n < m_FrameBufferCount; n++)
		{
			m_FenceValues[n] = m_FenceValues[m_FrameIndex];
		}

		m_FrameIndex = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE  hCPUHeapHandle = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_RTVDescriptorSize = m_pDeviceContext->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		for (UINT n = 0; n < m_FrameBufferCount; n++)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr = hCPUHeapHandle.ptr + m_RTVDescriptorSize * n;
			hr = m_pSwapChain->GetBuffer(n, IID_PPV_ARGS(&m_pRenderTargets[n]));
			ThrowIfFailed(hr, "Failed to get buffer in swap chain during descriptor heap initialization.");
			m_pDeviceContext->CreateRenderTargetView(m_pRenderTargets[n].Get(), nullptr, handle);


			WCHAR name[25];
			swprintf_s(name, L"Render Target %d", n);
			m_pRenderTargets[n]->SetName(name);
		}
	}

	void Direct3D12Context::CreateDSVs()
	{
		HRESULT hr;

		m_dsvHeap.Create(m_pDeviceContext.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 2);
		CD3DX12_HEAP_PROPERTIES heapProperty(D3D12_HEAP_TYPE_DEFAULT);

		// === Scene Depth Texture == //

		D3D12_RESOURCE_DESC SceneDepthResourceDesc = {};
		SceneDepthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		SceneDepthResourceDesc.Alignment = 0;
		SceneDepthResourceDesc.SampleDesc.Count = 1;
		SceneDepthResourceDesc.SampleDesc.Quality = 0;
		SceneDepthResourceDesc.MipLevels = 1;
		SceneDepthResourceDesc.Format = m_DsvFormat;
		SceneDepthResourceDesc.DepthOrArraySize = 1;
		SceneDepthResourceDesc.Width = (UINT)m_WindowWidth;
		SceneDepthResourceDesc.Height = (UINT)m_WindowHeight;
		SceneDepthResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		SceneDepthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE SceneDepthOptomizedClearValue = {};
		SceneDepthOptomizedClearValue.Format = m_DsvFormat;
		SceneDepthOptomizedClearValue.DepthStencil.Depth = m_DepthClearValue;
		SceneDepthOptomizedClearValue.DepthStencil.Stencil = 0;

		hr = m_pDeviceContext->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&SceneDepthResourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&SceneDepthOptomizedClearValue,
			IID_PPV_ARGS(&m_pDepthStencilTexture));
		m_pDepthStencilTexture->SetName(L"Scene Depth Stencil Buffer");
		if (FAILED(hr))
			IE_CORE_ERROR("Failed to create comitted resource for depth stencil view");

		D3D12_DEPTH_STENCIL_VIEW_DESC SceneDSVDesc = {};
		SceneDSVDesc.Texture2D.MipSlice = 0;
		SceneDSVDesc.Format = SceneDepthResourceDesc.Format;
		SceneDSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		SceneDSVDesc.Flags = D3D12_DSV_FLAG_NONE;

		m_pDeviceContext->CreateDepthStencilView(m_pDepthStencilTexture.Get(), &SceneDSVDesc, m_dsvHeap.hCPU(0));

		D3D12_SHADER_RESOURCE_VIEW_DESC SceneDSVSRV = {};
		SceneDSVSRV.Texture2D.MipLevels = SceneDepthResourceDesc.MipLevels;
		SceneDSVSRV.Texture2D.MostDetailedMip = 0;
		SceneDSVSRV.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		SceneDSVSRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SceneDSVSRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		m_pDeviceContext->CreateShaderResourceView(m_pDepthStencilTexture.Get(), &SceneDSVSRV, m_cbvsrvHeap.hCPU(4));

		// === Shadow Depth Texture === //

		D3D12_CLEAR_VALUE ShadowDepthOptomizedClearValue = {};
		ShadowDepthOptomizedClearValue.Format = m_ShadowMapFormat;
		ShadowDepthOptomizedClearValue.DepthStencil.Depth = m_DepthClearValue;
		ShadowDepthOptomizedClearValue.DepthStencil.Stencil = 0;

		D3D12_RESOURCE_DESC ShadowDepthResourceDesc = {};
		ShadowDepthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ShadowDepthResourceDesc.Alignment = 0;
		ShadowDepthResourceDesc.SampleDesc.Count = 1;
		ShadowDepthResourceDesc.SampleDesc.Quality = 0;
		ShadowDepthResourceDesc.MipLevels = 1;
		ShadowDepthResourceDesc.Format = m_ShadowMapFormat;
		ShadowDepthResourceDesc.DepthOrArraySize = 1;
		ShadowDepthResourceDesc.Width = m_ShadowMapWidth;
		ShadowDepthResourceDesc.Height = m_ShadowMapHeight;
		ShadowDepthResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ShadowDepthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_DEPTH_STENCIL_VIEW_DESC ShadowDepthDesc = {};
		ShadowDepthDesc.Texture2D.MipSlice = 0;
		ShadowDepthDesc.Format = ShadowDepthResourceDesc.Format;
		ShadowDepthDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		ShadowDepthDesc.Flags = D3D12_DSV_FLAG_NONE;

		hr = m_pDeviceContext->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&ShadowDepthResourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&ShadowDepthOptomizedClearValue,
			IID_PPV_ARGS(&m_pShadowDepthTexture));
		m_pShadowDepthTexture->SetName(L"Shadow Depth Buffer");
		if (FAILED(hr))
			IE_CORE_ERROR("Failed to create comitted resource for depth stencil view");

		m_pDeviceContext->CreateDepthStencilView(m_pShadowDepthTexture.Get(), &ShadowDepthDesc, m_dsvHeap.hCPU(1));

		D3D12_SHADER_RESOURCE_VIEW_DESC ShadowDSVSRV = {};
		ShadowDSVSRV.Texture2D.MipLevels = ShadowDepthResourceDesc.MipLevels;
		ShadowDSVSRV.Texture2D.MostDetailedMip = 0;
		ShadowDSVSRV.Format = DXGI_FORMAT_R32_FLOAT;
		ShadowDSVSRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		ShadowDSVSRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		m_pDeviceContext->CreateShaderResourceView(m_pShadowDepthTexture.Get(), &ShadowDSVSRV, m_cbvsrvHeap.hCPU(6));
	}

	void Direct3D12Context::CreateRTVs()
	{
		HRESULT hr;

		m_rtvHeap.Create(m_pDeviceContext.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 5);
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
			hr = m_pDeviceContext->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &clearVal, IID_PPV_ARGS(&m_pRenderTargetTextures[i]));
			ThrowIfFailed(hr, "Failed to create committed resource for RTV at index: " + std::to_string(i));
		}
		m_pRenderTargetTextures[0]->SetName(L"Render Target Texture Diffuse");
		m_pRenderTargetTextures[1]->SetName(L"Render Target Texture Normal");
		m_pRenderTargetTextures[2]->SetName(L"Render Target Texture (R)Roughness/(G)Metallic/(B)AO");
		m_pRenderTargetTextures[3]->SetName(L"Render Target Texture Position");
		m_pRenderTargetTextures[4]->SetName(L"Render Target Texture Light Pass Result");
		//resourceDesc.Format = m_RtvFormat[4];
		//clearVal.Format = m_RtvFormat[4];
		//hr = m_pLogicalDevice->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &clearVal, IID_PPV_ARGS(&m_pRenderTargetTextures[4]));
		//ThrowIfFailed(hr, "Failed to create committed resource for RTV at index: " + std::to_string(4));
		//m_pRenderTargetTextures[4]->SetName(L"Render Target Texture Light Pass Result");


		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Texture2D.MipSlice = 0;
		desc.Texture2D.PlaneSlice = 0;
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		for (int i = 0; i < m_NumRTV; i++) {
			desc.Format = m_RtvFormat[i];
			m_pDeviceContext->CreateRenderTargetView(m_pRenderTargetTextures[i].Get(), &desc, m_rtvHeap.hCPU(i));
		}
		//desc.Format = m_RtvFormat[4];
		//m_pLogicalDevice->CreateRenderTargetView(m_pRenderTargetTextures[4].Get(), &desc, m_rtvHeap.hCPU(4));

		//Create SRVs for Render Targets
		D3D12_SHADER_RESOURCE_VIEW_DESC descSRV = {};
		descSRV.Texture2D.MipLevels = resourceDesc.MipLevels;
		descSRV.Texture2D.MostDetailedMip = 0;
		descSRV.Format = resourceDesc.Format;
		descSRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		descSRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;


		for (int i = 0; i < m_NumRTV - 1; i++) {
			descSRV.Format = m_RtvFormat[i];
			m_pDeviceContext->CreateShaderResourceView(m_pRenderTargetTextures[i].Get(), &descSRV, m_cbvsrvHeap.hCPU(i));
		}
		m_pRenderTargetTextures[0]->SetName(L"Render Target SRV Albedo");
		m_pRenderTargetTextures[1]->SetName(L"Render Target SRV Normal");
		m_pRenderTargetTextures[2]->SetName(L"Render Target SRV (R)Roughness/(G)Metallic/(B)AO");
		m_pRenderTargetTextures[3]->SetName(L"Render Target SRV Position");

		// m_cbvsrvHeap.hCPU(4) is reserved for the depth stencil view
		descSRV.Format = m_RtvFormat[4];
		m_pDeviceContext->CreateShaderResourceView(m_pRenderTargetTextures[4].Get(), &descSRV, m_cbvsrvHeap.hCPU(5));
		m_pRenderTargetTextures[4]->SetName(L"Render Target SRV Light Pass Result");

	}

	void Direct3D12Context::CreateConstantBufferViews()
	{
		HRESULT hr = m_cbvsrvHeap.Create(m_pDeviceContext.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 35, true);
		ThrowIfFailed(hr, "Failed to create CBV SRV descriptor heap");

	}

	void Direct3D12Context::CreateRootSignature()
	{
		CD3DX12_DESCRIPTOR_RANGE range[12];
		range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 0); // G-Buffer inputs t0-t4

		range[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5); // PerObject texture inputs - Albedo
		range[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6); // PerObject texture inputs - Normal
		range[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 7); // PerObject texture inputs - Roughness
		range[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 8); // PerObject texture inputs - Metallic
		range[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 9); // PerObject texture inputs - AO
		range[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 10); // Shadow Depth texture

		range[7].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 11); // Sky - Irradiance
		range[8].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 12); // Sky - Environment Map
		range[9].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 13); // Sky - BRDF LUT
		range[10].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 14); // Sky - Diffuse

		range[11].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 15); // Post-FX Input

		CD3DX12_ROOT_PARAMETER rootParameters[17];
		rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);	  // Per-Object constant buffer
		rootParameters[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);		  // Per-Frame constant buffer
		rootParameters[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);		  // Light constant buffer
		rootParameters[3].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_PIXEL);	  // PostFx constant buffer
		rootParameters[4].InitAsConstantBufferView(4, 0, D3D12_SHADER_VISIBILITY_ALL);		  // Material Additives constant buffer
		rootParameters[5].InitAsDescriptorTable(1, &range[0], D3D12_SHADER_VISIBILITY_PIXEL); // G-Buffer inputs

		rootParameters[6].InitAsDescriptorTable(1, &range[1], D3D12_SHADER_VISIBILITY_PIXEL); // PerObject texture inputs - Albedo
		rootParameters[7].InitAsDescriptorTable(1, &range[2], D3D12_SHADER_VISIBILITY_PIXEL); // PerObject texture inputs - Normal
		rootParameters[8].InitAsDescriptorTable(1, &range[3], D3D12_SHADER_VISIBILITY_PIXEL); // PerObject texture inputs - Roughness
		rootParameters[9].InitAsDescriptorTable(1, &range[4], D3D12_SHADER_VISIBILITY_PIXEL); // PerObject texture inputs - Metallic
		rootParameters[10].InitAsDescriptorTable(1, &range[5], D3D12_SHADER_VISIBILITY_PIXEL); // PerObject texture inputs - AO
		rootParameters[11].InitAsDescriptorTable(1, &range[6], D3D12_SHADER_VISIBILITY_PIXEL); // Shadow Depth texture

		rootParameters[12].InitAsDescriptorTable(1, &range[7], D3D12_SHADER_VISIBILITY_PIXEL); // Sky - Irradiance
		rootParameters[13].InitAsDescriptorTable(1, &range[8], D3D12_SHADER_VISIBILITY_PIXEL); // Sky - Environment Map
		rootParameters[14].InitAsDescriptorTable(1, &range[9], D3D12_SHADER_VISIBILITY_PIXEL); // Sky - BRDF LUT
		rootParameters[15].InitAsDescriptorTable(1, &range[10], D3D12_SHADER_VISIBILITY_PIXEL); // Sky - Diffuse

		rootParameters[16].InitAsDescriptorTable(1, &range[11], D3D12_SHADER_VISIBILITY_PIXEL); // Final Image




		CD3DX12_ROOT_SIGNATURE_DESC descRootSignature;
		descRootSignature.Init(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		CD3DX12_STATIC_SAMPLER_DESC staticSamplers[2];
		// Shadow map sampler
		staticSamplers[0].Init(
			0,
			D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,
			0,
			1,
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
			0,
			0,
			D3D12_SHADER_VISIBILITY_PIXEL,
			0u
		);
		// Scene Sampler
		UINT maxAnisotropy = 16u;
		FLOAT minLOD = 0.0f;
		FLOAT maxLOD = 9.0f;
		FLOAT lodBias = 0.0f;
		staticSamplers[1].Init(
			1,
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			lodBias,
			maxAnisotropy,
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
			minLOD,
			maxLOD,
			D3D12_SHADER_VISIBILITY_PIXEL,
			0U
		);
		descRootSignature.NumStaticSamplers = _countof(staticSamplers);
		descRootSignature.pStaticSamplers = staticSamplers;

		ComPtr<ID3DBlob> rootSigBlob;
		ComPtr<ID3DBlob> errorBlob;

		HRESULT hr = D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, rootSigBlob.GetAddressOf(), errorBlob.GetAddressOf());
		ThrowIfFailed(hr, "Failed to serialize root signature");

		hr = m_pDeviceContext->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature));
		ThrowIfFailed(hr, "Failed to create root signature");
	}

	void Direct3D12Context::CreateShadowPassPSO()
	{
		HRESULT hr;

		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pPixelShader;

#if defined IE_DEBUG
		LPCWSTR VertexShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Shadow_Pass.vertex.cso";
		LPCWSTR PixelShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Shadow_Pass.pixel.cso";
#elif defined IE_RELEASE || defined IE_GAME_DIST || defined IE_ENGINE_DIST
		LPCWSTR VertexShaderFolder = L"Shadow_Pass.vertex.cso";
		LPCWSTR PixelShaderFolder = L"Shadow_Pass.pixel.cso";
#endif 

		hr = D3DReadFileToBlob(VertexShaderFolder, &pVertexShader);
		ThrowIfFailed(hr, "Failed to read Vertex Shader check log for more details.");
		hr = D3DReadFileToBlob(PixelShaderFolder, &pPixelShader);
		ThrowIfFailed(hr, "Failed to read Pixel Shader check log for more details.");

		D3D12_SHADER_BYTECODE VertexShaderBytecode = {};
		VertexShaderBytecode.BytecodeLength = pVertexShader->GetBufferSize();
		VertexShaderBytecode.pShaderBytecode = pVertexShader->GetBufferPointer();

		D3D12_SHADER_BYTECODE PixelShaderBytecode = {};
		PixelShaderBytecode.BytecodeLength = pPixelShader->GetBufferSize();
		PixelShaderBytecode.pShaderBytecode = pPixelShader->GetBufferPointer();

		D3D12_INPUT_ELEMENT_DESC InputLayout[5] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0  },
			{ "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0  },
			{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0  },
		};

		D3D12_INPUT_LAYOUT_DESC InputLayoutDesc = {};
		InputLayoutDesc.NumElements = sizeof(InputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
		InputLayoutDesc.pInputElementDescs = InputLayout;

		auto ShadowDepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		ShadowDepthStencilDesc.DepthEnable = true;
		ShadowDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		ShadowDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		ShadowDepthStencilDesc.StencilEnable = FALSE;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
		PsoDesc.InputLayout = InputLayoutDesc;
		PsoDesc.VS = VertexShaderBytecode;
		PsoDesc.PS = PixelShaderBytecode;
		PsoDesc.InputLayout.pInputElementDescs = InputLayout;
		PsoDesc.InputLayout.NumElements = _countof(InputLayout);
		PsoDesc.pRootSignature = m_pRootSignature.Get();
		PsoDesc.DepthStencilState = ShadowDepthStencilDesc;
		PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		PsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		PsoDesc.SampleMask = UINT_MAX;
		PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PsoDesc.DSVFormat = m_ShadowMapFormat;
		PsoDesc.SampleDesc.Count = 1;
		PsoDesc.NumRenderTargets = 0;
		PsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;

		hr = m_pDeviceContext->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&m_pPipelineStateObject_ShadowPass));
		ThrowIfFailed(hr, "Failed to create graphics pipeline state for shadow pass.");
		m_pPipelineStateObject_ShadowPass->SetName(L"PSO Shadow Pass");
	}

	void Direct3D12Context::CreateGeometryPassPSO()
	{
		HRESULT hr;

		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pPixelShader;

#if defined IE_DEBUG
		LPCWSTR vertexShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Geometry_Pass.vertex.cso";
		LPCWSTR pixelShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Geometry_Pass.pixel.cso";

#elif defined IE_RELEASE || defined IE_GAME_DIST || defined IE_ENGINE_DIST
		LPCWSTR vertexShaderFolder = L"Geometry_Pass.vertex.cso";
		LPCWSTR pixelShaderFolder = L"Geometry_Pass.pixel.cso";
#endif 

		hr = D3DReadFileToBlob(vertexShaderFolder, &pVertexShader);
		ThrowIfFailed(hr, "Failed to read Vertex Shader check log for more details.");
		hr = D3DReadFileToBlob(pixelShaderFolder, &pPixelShader);
		ThrowIfFailed(hr, "Failed to read Pixel Shader check log for more details.");


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

		D3D12_INPUT_LAYOUT_DESC InputLayoutDesc = {};
		InputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
		InputLayoutDesc.pInputElementDescs = inputLayout;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
		PsoDesc.VS = vertexShaderBytecode;
		PsoDesc.PS = pixelShaderBytecode;
		PsoDesc.InputLayout.pInputElementDescs = inputLayout;
		PsoDesc.InputLayout.NumElements = _countof(inputLayout);
		PsoDesc.pRootSignature = m_pRootSignature.Get();
		PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		PsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		PsoDesc.SampleMask = UINT_MAX;
		PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PsoDesc.NumRenderTargets = m_NumRTV;
		PsoDesc.RTVFormats[0] = m_RtvFormat[0];
		PsoDesc.RTVFormats[1] = m_RtvFormat[1];
		PsoDesc.RTVFormats[2] = m_RtvFormat[2];
		PsoDesc.RTVFormats[3] = m_RtvFormat[3];
		PsoDesc.DSVFormat = m_DsvFormat;
		PsoDesc.SampleDesc.Count = 1;

		hr = m_pDeviceContext->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&m_pPipelineStateObject_GeometryPass));
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
#elif defined IE_RELEASE || defined IE_GAME_DIST || defined IE_ENGINE_DIST
		LPCWSTR vertexShaderFolder = L"Skybox.vertex.cso";
		LPCWSTR pixelShaderFolder = L"Skybox.pixel.cso";
#endif 

		hr = D3DReadFileToBlob(vertexShaderFolder, &pVertexShader);
		ThrowIfFailed(hr, "Failed to compile Vertex Shader check log for more details.");
		hr = D3DReadFileToBlob(pixelShaderFolder, &pPixelShader);
		ThrowIfFailed(hr, "Failed to compile Pixel Shader check log for more details.");


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

		auto depthStencilStateDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		depthStencilStateDesc.DepthEnable = true;
		depthStencilStateDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilStateDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		auto rasterizerStateDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		rasterizerStateDesc.DepthClipEnable = true;
		rasterizerStateDesc.CullMode = D3D12_CULL_MODE_FRONT;
		rasterizerStateDesc.FillMode = D3D12_FILL_MODE_SOLID;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
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


		hr = m_pDeviceContext->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&m_pPipelineStateObject_SkyPass));
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
#elif defined IE_RELEASE || defined IE_GAME_DIST || defined IE_ENGINE_DIST
		LPCWSTR vertexShaderFolder = L"Light_Pass.vertex.cso";
		LPCWSTR pixelShaderFolder = L"Light_Pass.pixel.cso";
#endif 

		hr = D3DReadFileToBlob(vertexShaderFolder, &pVertexShader);
		ThrowIfFailed(hr, "Failed to compile Vertex Shader check log for more details.");
		hr = D3DReadFileToBlob(pixelShaderFolder, &pPixelShader);
		ThrowIfFailed(hr, "Failed to compile Pixel Shader check log for more details.");


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

		hr = m_pDeviceContext->CreateGraphicsPipelineState(&descPipelineState, IID_PPV_ARGS(&m_pPipelineStateObject_LightingPass));
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
#elif defined IE_RELEASE || defined IE_GAME_DIST || defined IE_ENGINE_DIST
		LPCWSTR vertexShaderFolder = L"PostFx.vertex.cso";
		LPCWSTR pixelShaderFolder = L"PostFx.pixel.cso";
#endif 

		hr = D3DReadFileToBlob(vertexShaderFolder, &pVertexShader);
		ThrowIfFailed(hr, "Failed to compile Vertex Shader check log for more details.");
		hr = D3DReadFileToBlob(pixelShaderFolder, &pPixelShader);
		ThrowIfFailed(hr, "Failed to compile Pixel Shader check log for more details.");


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
		descPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		descPipelineState.SampleDesc.Count = 1;

		hr = m_pDeviceContext->CreateGraphicsPipelineState(&descPipelineState, IID_PPV_ARGS(&m_pPipelineStateObject_PostFxPass));
		ThrowIfFailed(hr, "Failed to create graphics pipeline state for Post-Fx pass.");
		m_pPipelineStateObject_PostFxPass->SetName(L"PSO PostFx Pass");
	}

	void Direct3D12Context::CreateCommandAllocators()
	{
		HRESULT hr;

		// Scene Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_pDeviceContext->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pScenePassCommandAllocators[i]));
				m_pScenePassCommandAllocators[i]->SetName(L"Scene Pass Command Allocator");
				ThrowIfFailed(hr, "Failed to Scene Pass Create Command Allocator");
			}

			hr = m_pDeviceContext->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pScenePassCommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pScenePassCommandList));
			m_pScenePassCommandList->SetName(L"Scene Pass Command List");
			ThrowIfFailed(hr, "Failed to Scene Pass Create Command List");
		}

		// Shadow Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_pDeviceContext->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pShadowPassCommandAllocators[i]));
				m_pShadowPassCommandAllocators[i]->SetName(L"Graphics Command Allocator");
				ThrowIfFailed(hr, "Failed to Create Command Allocator");
			}

			hr = m_pDeviceContext->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pShadowPassCommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pShadowPassCommandList));
			m_pShadowPassCommandList->SetName(L"Shadow Pass Command List");
			ThrowIfFailed(hr, "Failed to Create Shadow Pass Command List");

			m_pShadowPassCommandList->Close();
			ID3D12CommandList* ppCommandLists[] = { m_pShadowPassCommandList.Get() };
			m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
			m_FenceValues[m_FrameIndex]++;
			HRESULT hr = m_pCommandQueue->Signal(m_pFence.Get(), m_FenceValues[m_FrameIndex]);
			if (FAILED(hr)) {
				IE_CORE_WARN("Command queue failed to signal.");
			}
			WaitForGPU();
		}
	}

	void Direct3D12Context::CreateFenceEvent()
	{
		HRESULT hr;
		hr = m_pDeviceContext->CreateFence(m_FenceValues[m_FrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
		ThrowIfFailed(hr, "Failed to create Fence Event");
		m_FenceValues[m_FrameIndex]++;


		m_FenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		if (m_FenceEvent == nullptr)
			THROW_COM_ERROR("Fence Event was nullptr");
	}

	void Direct3D12Context::LoadDemoAssets()
	{

	}

	void Direct3D12Context::CreateConstantBuffers()
	{
		HRESULT hr;

		// Light Constant buffer
		{
			hr = m_pDeviceContext->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_LightCBV));
			m_LightCBV->SetName(L"Constant Buffer Light Buffer Upload Resource Heap");
			ThrowIfFailed(hr, "Failed to create upload heap for light buffer upload resource heaps");
			CD3DX12_RANGE readRange(0, 0);
			hr = m_LightCBV->Map(0, &readRange, reinterpret_cast<void**>(&m_cbvLightBufferGPUAddress));
			ThrowIfFailed(hr, "Failed to map upload heap for light buffer upload resource heaps");
		}

		// PerObject Constant buffer
		for (int i = 0; i < m_FrameBufferCount; ++i)
		{
			hr = m_pDeviceContext->CreateCommittedResource(
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

		// PerObject Material Constant buffer
		for (int i = 0; i < m_FrameBufferCount; ++i)
		{
			hr = m_pDeviceContext->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_PerObjectMaterialAdditivesCBV[i]));
			m_PerObjectMaterialAdditivesCBV[i]->SetName(L"Constant Buffer Per-Object Material Upload Resource Heap");
			ThrowIfFailed(hr, "Failed to create upload heap for per-object upload resource heaps");
			CD3DX12_RANGE readRange(0, 0);
			hr = m_PerObjectMaterialAdditivesCBV[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_cbvPerObjectMaterialOverridesGPUAddress[i]));
			ThrowIfFailed(hr, "Failed to map upload heap for per-object material upload resource heaps");
		}

		// Per Frame
		{
			hr = m_pDeviceContext->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_PerFrameCBV));
			m_PerFrameCBV->SetName(L"Constant Buffer Per-Frame Upload Heap");
			ThrowIfFailed(hr, "Failed to create upload heap for per-frame upload resource heaps");
			CD3DX12_RANGE readRange(0, 0);
			hr = m_PerFrameCBV->Map(0, &readRange, reinterpret_cast<void**>(&m_cbvPerFrameGPUAddress));
			ThrowIfFailed(hr, "Failed to create map heap for per-frame upload resource heaps");
		}

		// Post-Fx
		{
			hr = m_pDeviceContext->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_PostFxCBV));
			m_PostFxCBV->SetName(L"Constant Buffer Per-Frame Upload Heap");
			ThrowIfFailed(hr, "Failed to create upload heap for per-frame upload resource heaps");
			CD3DX12_RANGE readRange(0, 0);
			hr = m_PostFxCBV->Map(0, &readRange, reinterpret_cast<void**>(&m_cbvPostFxGPUAddress));
			ThrowIfFailed(hr, "Failed to create map heap for per-frame upload resource heaps");
		}
	}

	void Direct3D12Context::CreateViewport()
	{
		m_ShadowPassViewPort.TopLeftX = 0;
		m_ShadowPassViewPort.TopLeftY = 0;
		m_ShadowPassViewPort.Width = static_cast<FLOAT>(m_ShadowMapWidth);
		m_ShadowPassViewPort.Height = static_cast<FLOAT>(m_ShadowMapHeight);
		m_ShadowPassViewPort.MinDepth = 0.0f;
		m_ShadowPassViewPort.MaxDepth = 1.0f;

		m_ScenePassViewPort.TopLeftX = 0;
		m_ScenePassViewPort.TopLeftY = 0;
		m_ScenePassViewPort.Width = static_cast<FLOAT>(m_WindowWidth);
		m_ScenePassViewPort.Height = static_cast<FLOAT>(m_WindowHeight);
		m_ScenePassViewPort.MinDepth = 0.0f;
		m_ScenePassViewPort.MaxDepth = 1.0f;
	}

	void Direct3D12Context::CreateScissorRect()
	{
		m_ShadowPassScissorRect.left = 0;
		m_ShadowPassScissorRect.top = 0;
		m_ShadowPassScissorRect.right = m_ShadowMapWidth;
		m_ShadowPassScissorRect.bottom = m_ShadowMapHeight;

		m_ScenePassScissorRect.left = 0;
		m_ScenePassScissorRect.top = 0;
		m_ScenePassScissorRect.right = m_WindowWidth;
		m_ScenePassScissorRect.bottom = m_WindowHeight;
	}

	void Direct3D12Context::CreateScreenQuad()
	{
		m_ScreenQuad.Init();
	}

	void Direct3D12Context::CloseCommandListAndSignalCommandQueue()
	{
		m_pScenePassCommandList->Close();
		ID3D12CommandList* ppCommandLists[] = { m_pScenePassCommandList.Get() };
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

		HRESULT hr = m_pDeviceContext->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCommandQueue));
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
		GetHardwareAdapter(m_pDxgiFactory.Get(), &m_pAdapter);

		HRESULT hr = D3D12CreateDevice(m_pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDeviceContext));
		ThrowIfFailed(hr, "Failed to create logical device.");
	}

	void Direct3D12Context::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
	{
		ComPtr<IDXGIAdapter1> pAdapter;
		*ppAdapter = nullptr;
		UINT currentVideoCardMemory = 0;
		DXGI_ADAPTER_DESC1 Desc;

		for (UINT AdapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(AdapterIndex, &pAdapter); ++AdapterIndex)
		{
			Desc = {};
			pAdapter->GetDesc1(&Desc);

			// Make sure we get the video card that is not a software adapter
			// and it has the most video memory
			if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE || Desc.DedicatedVideoMemory < currentVideoCardMemory) continue;

			if (SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
			{
				currentVideoCardMemory = static_cast<UINT>(Desc.DedicatedVideoMemory);
				if (*ppAdapter != nullptr) {
					(*ppAdapter)->Release();
				}
				*ppAdapter = pAdapter.Detach();

				IE_CORE_WARN("Found suitable Direct3D 12 graphics hardware: {0}", StringHelper::WideToString(std::wstring{ Desc.Description }));
			}
		}
		Desc = {};
		(*ppAdapter)->GetDesc1(&Desc);
		IE_CORE_WARN("\"{0}\" selected as Direct3D 11 graphics hardware.", StringHelper::WideToString(Desc.Description));
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
				m_pDeviceContext->CreateRenderTargetView(m_pRenderTargets[i].Get(), nullptr, m_rtvHeap.hCPU(i));
			}
		}

		// Re-Create Depth Stencil View
		{
			CreateDSVs();
			CreateRTVs();
			m_RTVDescriptorHeap.Reset();
			CreateRenderTargetViewDescriptorHeap();
		}

		// Recreate Camera Projection Matrix
		{
			if (!m_pWorldCamera->GetIsOrthographic()) {
				m_pWorldCamera->SetPerspectiveProjectionValues(m_pWorldCamera->GetFOV(), static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight), m_pWorldCamera->GetNearZ(), m_pWorldCamera->GetFarZ());
			}
		}

	}

	void Direct3D12Context::UpdateViewAndScissor()
	{
		m_ScenePassViewPort.TopLeftX = 0.0f;
		m_ScenePassViewPort.TopLeftY = 0.0f;
		m_ScenePassViewPort.Width = static_cast<FLOAT>(m_WindowWidth);
		m_ScenePassViewPort.Height = static_cast<FLOAT>(m_WindowHeight);

		m_ScenePassScissorRect.left = static_cast<LONG>(m_ScenePassViewPort.TopLeftX);
		m_ScenePassScissorRect.right = static_cast<LONG>(m_ScenePassViewPort.TopLeftX + m_ScenePassViewPort.Width);
		m_ScenePassScissorRect.top = static_cast<LONG>(m_ScenePassViewPort.TopLeftY);
		m_ScenePassScissorRect.bottom = static_cast<LONG>(m_ScenePassViewPort.TopLeftX + m_ScenePassViewPort.Height);
	}

	void ScreenQuad::Init()
	{
		HRESULT hr;
		Direct3D12Context* graphicsContext = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());

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
		hr = graphicsContext->GetDeviceContext().CreateCommittedResource(
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
		hr = graphicsContext->GetDeviceContext().CreateCommittedResource(
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

		UpdateSubresources(&graphicsContext->GetScenePassCommandList(), m_VertexBuffer.Get(), vBufferUploadHeap, 0, 0, 1, &vertexData);

		graphicsContext->GetScenePassCommandList().ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_VertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = sizeof(ScreenSpaceVertex);
		m_VertexBufferView.SizeInBytes = vBufferSize;

	}

	void ScreenQuad::OnRender(ComPtr<ID3D12GraphicsCommandList> commandList)
	{
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		commandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
		commandList->DrawInstanced(m_NumVerticies, 1, 0, 0);
	}


}