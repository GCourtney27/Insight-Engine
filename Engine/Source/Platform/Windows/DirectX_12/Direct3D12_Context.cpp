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
#include "Platform/Windows/DirectX_12/Geometry/D3D12_Sphere_Renderer.h"

namespace Insight {


	Direct3D12Context::Direct3D12Context(WindowsWindow* WindowHandle)
		: m_pWindowHandle(&WindowHandle->GetWindowHandleReference()),
		m_pWindow(WindowHandle),
		Renderer(WindowHandle->GetWidth(), WindowHandle->GetHeight(), false)
	{
		IE_CORE_ASSERT(WindowHandle, "Window handle is NULL, cannot initialize D3D 12 context with NULL window handle.");
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

		m_RTHelper.OnDestroy();

		if (!m_AllowTearing) {
			m_pSwapChain->SetFullscreenState(false, NULL);
		}

		if (!CloseHandle(m_FenceEvent)) {
			IE_CORE_ERROR("Failed to close GPU handle while cleaning up the D3D 12 context.");
		}
	}

	bool Direct3D12Context::Init_Impl()
	{
		IE_CORE_INFO("Renderer: D3D 12");

		try {
			CreateDXGIFactory();
			CreateDevice();

			CreateCommandQueue();
			CreateFenceEvent();
			CreateCommandAllocators();

			CreateConstantBuffers();
			CreateCBVs();
			CreateSRVs();

			PIXBeginEvent(m_pCommandQueue.Get(), 0, L"D3D 12 context Setup");
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
					CreateDeferredShadingRootSignature();
					CreateForwardShadingRootSignature();
					CreateShadowPassPSO();
					CreateGeometryPassPSO();
					CreateLightPassPSO();
					CreateSkyPassPSO();
					CreateTransparencyPassPSO();
					CreatePostEffectsPassPSO();
					m_RTHelper.OnInit(m_pDevice, m_pRayTracePass_CommandList, { m_WindowWidth, m_WindowHeight }, this);
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

	void Direct3D12Context::Destroy_Impl()
	{
		Cleanup();
	}

	bool Direct3D12Context::PostInit_Impl()
	{
		CloseCommandListAndSignalCommandQueue();
		m_pWorldCamera = &ACamera::Get();

		return true;
	}

	void Direct3D12Context::OnUpdate_Impl(const float DeltaMs)
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		// Send Per-Frame Data to GPU
		XMFLOAT4X4 viewFloat;
		XMStoreFloat4x4(&viewFloat, XMMatrixTranspose(m_pWorldCamera->GetViewMatrix()));
		XMFLOAT4X4 projectionFloat;
		XMStoreFloat4x4(&projectionFloat, XMMatrixTranspose(m_pWorldCamera->GetProjectionMatrix()));
		XMVECTOR det;
		XMMATRIX invView = XMMatrixTranspose(XMMatrixInverse(&det, m_pWorldCamera->GetViewMatrix()));
		XMMATRIX invProjection = XMMatrixTranspose(XMMatrixInverse(&det, m_pWorldCamera->GetProjectionMatrix()));
		XMFLOAT4X4 invViewFloat;
		XMStoreFloat4x4(&invViewFloat, invView);
		XMFLOAT4X4 invProjectionFloat;
		XMStoreFloat4x4(&invProjectionFloat, invProjection);

		m_PerFrameData.view = viewFloat;
		m_PerFrameData.projection = projectionFloat;
		m_PerFrameData.inverseView = invViewFloat;
		m_PerFrameData.inverseProjection = invProjectionFloat;
		m_PerFrameData.cameraPosition = m_pWorldCamera->GetTransformRef().GetPosition();
		m_PerFrameData.DeltaMs = DeltaMs;
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

		m_RTHelper.UpdateCBVs();

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

	void Direct3D12Context::OnPreFrameRender_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		// Reset Command Allocators
		ThrowIfFailed(m_pScenePass_CommandAllocators[m_FrameIndex]->Reset(),
			"Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Scene Pass");

		ThrowIfFailed(m_pShadowPass_CommandAllocators[m_FrameIndex]->Reset(),
			"Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Shadow Pass");

		ThrowIfFailed(m_pTransparencyPass_CommandAllocators[m_FrameIndex]->Reset(),
			"Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Transparency Pass");

		ThrowIfFailed(m_pRayTracePass_CommandAllocators[m_FrameIndex]->Reset(),
			"Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Ray Trace Pass");

		ThrowIfFailed(m_pPostEffectsPass_CommandAllocators[m_FrameIndex]->Reset(),
			"Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Post-Process Pass");

		// Reset Command Lists
		ThrowIfFailed(m_pScenePass_CommandList->Reset(m_pScenePass_CommandAllocators[m_FrameIndex].Get(), m_pGeometryPass_PSO.Get()),
			"Failed to reset command list in Direct3D12Context::OnPreFrameRender for Scene Pass");

		ThrowIfFailed(m_pShadowPass_CommandList->Reset(m_pShadowPass_CommandAllocators[m_FrameIndex].Get(), m_pShadowPass_PSO.Get()),
			"Failed to reset command list in Direct3D12Context::OnPreFrameRender for Shadow Pass");

		ThrowIfFailed(m_pTransparencyPass_CommandList->Reset(m_pTransparencyPass_CommandAllocators[m_FrameIndex].Get(), m_pTransparency_PSO.Get()),
			"Failed to reset command list in Direct3D12Context::OnPreFrameRender for Transparency Pass");

		ThrowIfFailed(m_pRayTracePass_CommandList->Reset(m_pRayTracePass_CommandAllocators[m_FrameIndex].Get(), nullptr),
			"Failed to reset command list in Direct3D12Context::OnPreFrameRender for Ray Trace Pass");

		ThrowIfFailed(m_pPostEffectsPass_CommandList->Reset(m_pPostEffectsPass_CommandAllocators[m_FrameIndex].Get(), m_pPostFxPass_PSO.Get()),
			"Failed to reset command list in Direct3D12Context::OnPreFrameRender for Transparency Pass");


		m_pScenePass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GetRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		const float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_pScenePass_CommandList->ClearRenderTargetView(GetRenderTargetView(), ClearColor, 0, nullptr);

		// Reset Scene Pass
		PIXBeginEvent(m_pScenePass_CommandList.Get(), 0, L"Resetting Scene Pass Command List");
		{
			m_pScenePass_CommandList->RSSetScissorRects(1, &m_ScenePassScissorRect);
			m_pScenePass_CommandList->RSSetViewports(1, &m_ScenePassViewPort);
		}
		PIXEndEvent(m_pScenePass_CommandList.Get());

		// Reset Shadow Pass
		PIXBeginEvent(m_pShadowPass_CommandList.Get(), 0, L"Resetting Shadow Pass Command List");
		{
			m_pShadowPass_CommandList->RSSetScissorRects(1, &m_ShadowPassScissorRect);
			m_pShadowPass_CommandList->RSSetViewports(1, &m_ShadowPassViewPort);
			m_pShadowPass_CommandList->OMSetRenderTargets(0, nullptr, FALSE, &m_dsvHeap.hCPU(1));
		}
		PIXEndEvent(m_pShadowPass_CommandList.Get());
	}

	void Direct3D12Context::OnRender_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		// Render Shadows
		m_pActiveCommandList = m_pShadowPass_CommandList;
		BindShadowPass();

		// Render Scene
		m_pActiveCommandList = m_pScenePass_CommandList;
		BindGeometryPass();

		// Transparency Forward Pass
		m_pActiveCommandList = m_pTransparencyPass_CommandList;
		BindTransparencyPass();

		if (m_IsRayTraceEnabled) {
			m_pActiveCommandList = m_pRayTracePass_CommandList;
			BindRayTracePass();
		}
	}

	void Direct3D12Context::BindShadowPass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		PIXBeginEvent(m_pShadowPass_CommandList.Get(), 0, L"Rendering Shadow Pass");
		{
			ID3D12DescriptorHeap* ppHeaps[] = { m_cbvsrvHeap.pDH.Get() };
			m_pShadowPass_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			m_pShadowPass_CommandList->SetPipelineState(m_pShadowPass_PSO.Get());
			m_pShadowPass_CommandList->SetGraphicsRootSignature(m_pDeferredShadingPass_RootSignature.Get());
			m_pShadowPass_CommandList->ClearDepthStencilView(m_dsvHeap.hCPU(1), D3D12_CLEAR_FLAG_DEPTH, m_DepthClearValue, 0xff, 0, nullptr);

			m_pShadowPass_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_pShadowPass_CommandList->SetGraphicsRootConstantBufferView(1, m_PerFrameCBV->GetGPUVirtualAddress());
			m_pShadowPass_CommandList->SetGraphicsRootConstantBufferView(2, m_LightCBV->GetGPUVirtualAddress());

			// TODO Shadow pass logic here put this on another thread
			GeometryManager::Render(eRenderPass::RenderPass_Shadow);
		}
		PIXEndEvent(m_pShadowPass_CommandList.Get());
	}

	void Direct3D12Context::BindGeometryPass(bool setPSO)
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		PIXBeginEvent(m_pScenePass_CommandList.Get(), 0, L"Rendering Scene Pass");
		{
			ID3D12DescriptorHeap* ppHeaps[] = { m_cbvsrvHeap.pDH.Get() };
			m_pScenePass_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			if (setPSO) {
				m_pScenePass_CommandList->SetPipelineState(m_pGeometryPass_PSO.Get());
			}

			for (int i = 0; i < m_NumRTV - 1; i++) {
				m_pScenePass_CommandList->ClearRenderTargetView(m_rtvHeap.hCPU(i), m_ClearColor, 0, nullptr);
			}

			m_pScenePass_CommandList->ClearDepthStencilView(m_dsvHeap.hCPU(0), D3D12_CLEAR_FLAG_DEPTH, m_DepthClearValue, 0xff, 0, nullptr);

			m_pScenePass_CommandList->OMSetRenderTargets(m_NumRTV, &m_rtvHeap.hCPUHeapStart, TRUE, &m_dsvHeap.hCPU(0));
			m_pScenePass_CommandList->SetGraphicsRootSignature(m_pDeferredShadingPass_RootSignature.Get());
			// Set Scene Depth Texture
			m_pScenePass_CommandList->SetGraphicsRootDescriptorTable(5, m_cbvsrvHeap.hGPU(0));
			// Set Shadow Depth Texture
			m_pScenePass_CommandList->SetGraphicsRootDescriptorTable(11, m_cbvsrvHeap.hGPU(6));

			{
				m_pScenePass_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				m_pScenePass_CommandList->SetGraphicsRootConstantBufferView(2, m_LightCBV->GetGPUVirtualAddress());
				m_pScenePass_CommandList->SetGraphicsRootConstantBufferView(1, m_PerFrameCBV->GetGPUVirtualAddress());
			}

			GeometryManager::Render(eRenderPass::RenderPass_Scene);
		}
		PIXEndEvent(m_pScenePass_CommandList.Get());
	}

	void Direct3D12Context::OnMidFrameRender_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		BindLightingPass();

		BindSkyPass();

		BindPostFxPass();
	}

	void Direct3D12Context::BindLightingPass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		PIXBeginEvent(m_pScenePass_CommandList.Get(), 0, L"Rendering Light Pass");
		{
			m_pScenePass_CommandList->OMSetRenderTargets(1, &m_rtvHeap.hCPU(4), TRUE, nullptr);

			if (m_pSkyLight) {
				m_pSkyLight->BindCubeMaps(true);
			}

			for (unsigned int i = 0; i < m_NumRTV - 1; ++i) {
				m_pScenePass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargetTextures[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
			}

			m_pScenePass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));

			m_pScenePass_CommandList->SetPipelineState(m_pLightingPass_PSO.Get());

			m_ScreenQuad.OnRender(m_pScenePass_CommandList);
		}
		PIXEndEvent(m_pScenePass_CommandList.Get());
	}

	void Direct3D12Context::BindSkyPass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;
		PIXBeginEvent(m_pScenePass_CommandList.Get(), 0, L"Rendering Sky Pass");
		{
			if (m_pSkySphere) {
				m_pScenePass_CommandList->OMSetRenderTargets(1, &m_rtvHeap.hCPU(4), TRUE, &m_dsvHeap.hCPU(0));

				m_pScenePass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

				m_pScenePass_CommandList->SetPipelineState(m_pSkyPass_PSO.Get());

				m_pSkySphere->RenderSky(m_pScenePass_CommandList);
			}
			else {
				m_pScenePass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
			}
		}
		PIXEndEvent(m_pScenePass_CommandList.Get());
	}

	void Direct3D12Context::BindTransparencyPass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		PIXBeginEvent(m_pTransparencyPass_CommandList.Get(), 0, "Rendering Transparency Pass");
		{
			ID3D12DescriptorHeap* ppHeaps[] = { m_cbvsrvHeap.pDH.Get() };
			m_pTransparencyPass_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			m_pTransparencyPass_CommandList->SetPipelineState(m_pTransparency_PSO.Get());
			m_pTransparencyPass_CommandList->SetGraphicsRootSignature(m_pForwardShadingPass_RootSignature.Get());

			m_pTransparencyPass_CommandList->RSSetScissorRects(1, &m_ScenePassScissorRect);
			m_pTransparencyPass_CommandList->RSSetViewports(1, &m_ScenePassViewPort);
			m_pTransparencyPass_CommandList->OMSetRenderTargets(1, &m_rtvHeap.hCPU(4), TRUE, &m_dsvHeap.hCPU(0));

			m_pTransparencyPass_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_pTransparencyPass_CommandList->SetGraphicsRootConstantBufferView(1, m_PerFrameCBV->GetGPUVirtualAddress());
			m_pTransparencyPass_CommandList->SetGraphicsRootConstantBufferView(2, m_LightCBV->GetGPUVirtualAddress());
			m_pTransparencyPass_CommandList->SetGraphicsRootDescriptorTable(4, m_cbvsrvHeap.hGPU(4));

			GeometryManager::Render(eRenderPass::RenderPass_Transparency);
		}
		PIXEndEvent(m_pTransparencyPass_CommandList.Get());
	}

	void Direct3D12Context::BindRayTracePass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		PIXBeginEvent(m_pRayTracePass_CommandList.Get(), 0, L"Rendering Ray Trace Pass");
		{
			//ID3D12DescriptorHeap* ppHeaps[] = { m_cbvsrvHeap.pDH.Get() };
			//m_pRayTracePass_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			m_RTHelper.SetCommonPipeline();
			
			//m_pRayTracePass_CommandList->SetGraphicsRootConstantBufferView(0, m_PerFrameCBV->GetGPUVirtualAddress());
			
			m_pRayTracePass_CommandList->CopyResource(m_RayTraceOutput_SRV.Get(), m_RTHelper.GetOutputBuffer());

			m_RTHelper.TraceScene();

		}
		PIXEndEvent(m_pRayTracePass_CommandList.Get());
	}

	void Direct3D12Context::BindPostFxPass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		PIXBeginEvent(m_pPostEffectsPass_CommandList.Get(), 0, L"Rendering Post-Process Pass");
		{
			ID3D12DescriptorHeap* ppHeaps[] = { m_cbvsrvHeap.pDH.Get() };
			m_pPostEffectsPass_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			m_pPostEffectsPass_CommandList->OMSetRenderTargets(1, &GetRenderTargetView(), TRUE, nullptr);
			m_pPostEffectsPass_CommandList->RSSetScissorRects(1, &m_ScenePassScissorRect);
			m_pPostEffectsPass_CommandList->RSSetViewports(1, &m_ScenePassViewPort);

			m_pPostEffectsPass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
			m_pPostEffectsPass_CommandList->SetGraphicsRootSignature(m_pDeferredShadingPass_RootSignature.Get());

			m_pPostEffectsPass_CommandList->SetPipelineState(m_pPostFxPass_PSO.Get());
			m_pPostEffectsPass_CommandList->SetGraphicsRootDescriptorTable(16, m_cbvsrvHeap.hGPU(5)); // Light Pass result
			m_pPostEffectsPass_CommandList->SetGraphicsRootDescriptorTable(17, m_cbvsrvHeap.hGPU(6)); // Ray Trace Result
			m_pPostEffectsPass_CommandList->SetGraphicsRootConstantBufferView(1, m_PerFrameCBV->GetGPUVirtualAddress());
			m_pPostEffectsPass_CommandList->SetGraphicsRootConstantBufferView(3, m_PostFxCBV->GetGPUVirtualAddress());


			m_ScreenQuad.OnRender(m_pPostEffectsPass_CommandList);
		}
		PIXEndEvent(m_pPostEffectsPass_CommandList.Get());
	}

	void Direct3D12Context::ExecuteDraw_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		// Prepare the buffers for next frame
		{
			for (unsigned int i = 0; i < m_NumRTV - 1; ++i) {
				m_pPostEffectsPass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargetTextures[i].Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
			}
			m_pPostEffectsPass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
		}

		// Prepare render target to be presented
		m_pPostEffectsPass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GetRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		ThrowIfFailed(m_pShadowPass_CommandList->Close(), "Failed to close the command list for D3D 12 context shadow pass.");
		ThrowIfFailed(m_pScenePass_CommandList->Close(), "Failed to close command list for D3D 12 context scene pass.");
		ThrowIfFailed(m_pTransparencyPass_CommandList->Close(), "Failed to close the command list for D3D 12 context transparency pass.");
		ThrowIfFailed(m_pPostEffectsPass_CommandList->Close(), "Failed to close the command list for D3D 12 context post-process pass.");
		ThrowIfFailed(m_pRayTracePass_CommandList->Close(), "Failed to close the command list for D3D 12 context ray trace pass.");

		ID3D12CommandList* ppCommandLists[] = {
			m_pShadowPass_CommandList.Get(), // Execure shadow pass first because we'll need the depth textures for the light pass
			m_pScenePass_CommandList.Get(),
			m_pTransparencyPass_CommandList.Get(),
			m_pRayTracePass_CommandList.Get(),
			m_pPostEffectsPass_CommandList.Get(),
		};
		m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		WaitForGPU();
	}

	void Direct3D12Context::SwapBuffers_Impl()
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

	void Direct3D12Context::OnWindowResize_Impl()
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
				ThrowIfFailed(hr, "Failed to resize swap chain buffers for D3D 12 context.");

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

	void Direct3D12Context::OnWindowFullScreen_Impl()
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
					ThrowIfFailed(m_pSwapChain->GetContainingOutput(&pOutput), "Failed to get containing output while switching to fullscreen mode in D3D 12 context.");
					DXGI_OUTPUT_DESC Desc;
					ThrowIfFailed(pOutput->GetDesc(&Desc), "Failed to get description from output while switching to fullscreen mode in D3D 12 context.");
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

	void Direct3D12Context::OnShaderReload_Impl()
	{
		// Relaoad Post Process
		{
			m_pPostFxPass_PSO.Reset();
			CreatePostEffectsPassPSO();
		}

		// Reload Geometry Pass
		{
			m_pGeometryPass_PSO.Reset();
			CreateGeometryPassPSO();
		}

		// Reload Light Pass
		{
			m_pLightingPass_PSO.Reset();
			CreateLightPassPSO();
		}

		// Reload Sky Pass
		{
			m_pSkyPass_PSO.Reset();
			CreateSkyPassPSO();
		}

		// Reload Transparency Pass
		{
			m_pTransparency_PSO.Reset();
			CreateTransparencyPassPSO();
		}
	}

	void Direct3D12Context::SetVertexBuffers_Impl(uint32_t StartSlot, uint32_t NumBuffers, ieVertexBuffer* pBuffers)
	{
		m_pActiveCommandList->IASetVertexBuffers(StartSlot, NumBuffers, reinterpret_cast<D3D12VertexBuffer*>(pBuffers)->GetVertexBufferView());
	}

	void Direct3D12Context::SetIndexBuffer_Impl(ieIndexBuffer* pBuffer)
	{
		m_pActiveCommandList->IASetIndexBuffer(&reinterpret_cast<D3D12IndexBuffer*>(pBuffer)->GetIndexBufferView());
	}

	void Direct3D12Context::DrawIndexedInstanced_Impl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation)
	{
		m_pActiveCommandList->DrawIndexedInstanced(IndexCountPerInstance, NumInstances, StartIndexLocation, BaseVertexLoaction, StartInstanceLocation);
	}

	void Direct3D12Context::RenderSkySphere_Impl()
	{
		m_SkySphere->Render(m_pScenePass_CommandList);
	}

	bool Direct3D12Context::CreateSkybox_Impl()
	{
		m_SkySphere = new ieD3D12SphereRenderer();
		m_SkySphere->Init(10, 20, 20);
		return true;
	}

	void Direct3D12Context::DestroySkybox_Impl()
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

		hr = m_pDevice->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&m_RTVDescriptorHeap));
		m_RTVDescriptorHeap->SetName(L"Render Target View Descriptor Heap");

		// All pending GPU work was already finished. Update the tracked fence values
		// to the last value signaled.
		for (UINT n = 0; n < m_FrameBufferCount; n++)
		{
			m_FenceValues[n] = m_FenceValues[m_FrameIndex];
		}

		m_FrameIndex = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE  hCPUHeapHandle = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_RTVDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		for (UINT n = 0; n < m_FrameBufferCount; n++)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr = hCPUHeapHandle.ptr + m_RTVDescriptorSize * n;
			hr = m_pSwapChain->GetBuffer(n, IID_PPV_ARGS(&m_pRenderTargets[n]));
			ThrowIfFailed(hr, "Failed to get buffer in swap chain during descriptor heap initialization for D3D 12 context.");
			m_pDevice->CreateRenderTargetView(m_pRenderTargets[n].Get(), nullptr, handle);


			WCHAR name[25];
			swprintf_s(name, L"Render Target %d", n);
			m_pRenderTargets[n]->SetName(name);
		}
	}

	void Direct3D12Context::CreateDSVs()
	{
		HRESULT hr;

		m_dsvHeap.Create(m_pDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 2);
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

		hr = m_pDevice->CreateCommittedResource(
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

		m_pDevice->CreateDepthStencilView(m_pDepthStencilTexture.Get(), &SceneDSVDesc, m_dsvHeap.hCPU(0));

		D3D12_SHADER_RESOURCE_VIEW_DESC SceneDSVSRV = {};
		SceneDSVSRV.Texture2D.MipLevels = SceneDepthResourceDesc.MipLevels;
		SceneDSVSRV.Texture2D.MostDetailedMip = 0;
		SceneDSVSRV.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		SceneDSVSRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SceneDSVSRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		m_pDevice->CreateShaderResourceView(m_pDepthStencilTexture.Get(), &SceneDSVSRV, m_cbvsrvHeap.hCPU(4));

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

		hr = m_pDevice->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&ShadowDepthResourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&ShadowDepthOptomizedClearValue,
			IID_PPV_ARGS(&m_pShadowDepthTexture));
		m_pShadowDepthTexture->SetName(L"Shadow Depth Buffer");
		if (FAILED(hr))
			IE_CORE_ERROR("Failed to create comitted resource for depth stencil view");

		m_pDevice->CreateDepthStencilView(m_pShadowDepthTexture.Get(), &ShadowDepthDesc, m_dsvHeap.hCPU(1));

		D3D12_SHADER_RESOURCE_VIEW_DESC ShadowDSVSRV = {};
		ShadowDSVSRV.Texture2D.MipLevels = ShadowDepthResourceDesc.MipLevels;
		ShadowDSVSRV.Texture2D.MostDetailedMip = 0;
		ShadowDSVSRV.Format = DXGI_FORMAT_R32_FLOAT;
		ShadowDSVSRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		ShadowDSVSRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		m_pDevice->CreateShaderResourceView(m_pShadowDepthTexture.Get(), &ShadowDSVSRV, m_cbvsrvHeap.hCPU(8));
	}

	void Direct3D12Context::CreateRTVs()
	{
		HRESULT hr;

		m_rtvHeap.Create(m_pDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 5);
		CD3DX12_HEAP_PROPERTIES HeapProperties(D3D12_HEAP_TYPE_DEFAULT);

		D3D12_RESOURCE_DESC ResourceDesc = {};
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Alignment = 0;
		ResourceDesc.SampleDesc.Count = 1;
		ResourceDesc.SampleDesc.Quality = 0;
		ResourceDesc.MipLevels = 1;

		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.Width = (UINT)m_WindowWidth;
		ResourceDesc.Height = (UINT)m_WindowHeight;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;


		D3D12_CLEAR_VALUE ClearVal;
		ClearVal.Color[0] = m_ClearColor[0];
		ClearVal.Color[1] = m_ClearColor[1];
		ClearVal.Color[2] = m_ClearColor[2];
		ClearVal.Color[3] = m_ClearColor[3];

		for (int i = 0; i < m_NumRTV; i++) {
			ResourceDesc.Format = m_RtvFormat[i];
			ClearVal.Format = m_RtvFormat[i];
			hr = m_pDevice->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &ClearVal, IID_PPV_ARGS(&m_pRenderTargetTextures[i]));
			ThrowIfFailed(hr, "Failed to create committed resource for RTV at index: " + std::to_string(i));
		}
		m_pRenderTargetTextures[0]->SetName(L"Render Target Texture Diffuse");
		m_pRenderTargetTextures[1]->SetName(L"Render Target Texture Normal");
		m_pRenderTargetTextures[2]->SetName(L"Render Target Texture (R)Roughness/(G)Metallic/(B)AO");
		m_pRenderTargetTextures[3]->SetName(L"Render Target Texture Position");
		m_pRenderTargetTextures[4]->SetName(L"Render Target Texture Light Pass Result");


		D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = {};
		RTVDesc.Texture2D.MipSlice = 0;
		RTVDesc.Texture2D.PlaneSlice = 0;
		RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		for (int i = 0; i < m_NumRTV; i++) {
			RTVDesc.Format = m_RtvFormat[i];
			m_pDevice->CreateRenderTargetView(m_pRenderTargetTextures[i].Get(), &RTVDesc, m_rtvHeap.hCPU(i));
		}

		//Create SRVs for Render Targets
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Texture2D.MipLevels = ResourceDesc.MipLevels;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Format = ResourceDesc.Format;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;


		for (int i = 0; i < m_NumRTV - 1; i++) {
			SRVDesc.Format = m_RtvFormat[i];
			m_pDevice->CreateShaderResourceView(m_pRenderTargetTextures[i].Get(), &SRVDesc, m_cbvsrvHeap.hCPU(i));
		}
		m_pRenderTargetTextures[0]->SetName(L"Render Target SRV Albedo");
		m_pRenderTargetTextures[1]->SetName(L"Render Target SRV Normal");
		m_pRenderTargetTextures[2]->SetName(L"Render Target SRV (R)Roughness/(G)Metallic/(B)AO");
		m_pRenderTargetTextures[3]->SetName(L"Render Target SRV Position");

		SRVDesc.Format = m_RtvFormat[4];
		m_pDevice->CreateShaderResourceView(m_pRenderTargetTextures[4].Get(), &SRVDesc, m_cbvsrvHeap.hCPU(5));
		m_pRenderTargetTextures[4]->SetName(L"Render Target SRV Light Pass Result");

	}

	void Direct3D12Context::CreateCBVs()
	{
		HRESULT hr = m_cbvsrvHeap.Create(m_pDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 45, true);
		ThrowIfFailed(hr, "Failed to create CBV SRV descriptor heap");
	}

	void Direct3D12Context::CreateSRVs()
	{
		HRESULT hr;
		CD3DX12_HEAP_PROPERTIES HeapProperties(D3D12_HEAP_TYPE_DEFAULT);

		D3D12_RESOURCE_DESC ResourceDesc = {};
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Alignment = 0;
		ResourceDesc.SampleDesc.Count = 1;
		ResourceDesc.SampleDesc.Quality = 0;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.Width = (UINT)m_WindowWidth;
		ResourceDesc.Height = (UINT)m_WindowHeight;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		hr = m_pDevice->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, NULL, IID_PPV_ARGS(&m_RayTraceOutput_SRV));
		ThrowIfFailed(hr, "Failed to create ray trace output srv");

		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDesc.Texture2D.MipLevels = 1U;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		m_pDevice->CreateShaderResourceView(m_RayTraceOutput_SRV.Get(), &SRVDesc, m_cbvsrvHeap.hCPU(6));
	}

	void Direct3D12Context::CreateDeferredShadingRootSignature()
	{
		CD3DX12_DESCRIPTOR_RANGE DescriptorRanges[13] = {};
		DescriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 0); // G-Buffer inputs t0-t4

		DescriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);  // PerObject texture inputs - Albedo
		DescriptorRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6);  // PerObject texture inputs - Normal
		DescriptorRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 7);  // PerObject texture inputs - Roughness
		DescriptorRanges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 8);  // PerObject texture inputs - Metallic
		DescriptorRanges[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 9);  // PerObject texture inputs - AO

		DescriptorRanges[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 10); // Shadow Depth texture

		DescriptorRanges[7].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 11);  // Sky - Irradiance
		DescriptorRanges[8].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 12);  // Sky - Environment Map
		DescriptorRanges[9].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 13);  // Sky - BRDF LUT
		DescriptorRanges[10].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 14); // Sky - Diffuse

		DescriptorRanges[11].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 15); // Post-FX input deferred light pass result
		DescriptorRanges[12].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 16); // Post-FX input raytrace pass result

		CD3DX12_ROOT_PARAMETER RootParameters[18] = {};
		RootParameters[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);	  // Per-Object constant buffer
		RootParameters[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);		  // Per-Frame constant buffer
		RootParameters[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);		  // Light constant buffer
		RootParameters[3].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_PIXEL);	  // PostFx constant buffer
		RootParameters[4].InitAsConstantBufferView(4, 0, D3D12_SHADER_VISIBILITY_ALL);		  // Material Additives constant buffer
		RootParameters[5].InitAsDescriptorTable(1, &DescriptorRanges[0], D3D12_SHADER_VISIBILITY_PIXEL); // G-Buffer inputs

		RootParameters[6].InitAsDescriptorTable(1, &DescriptorRanges[1], D3D12_SHADER_VISIBILITY_PIXEL);  // PerObject texture inputs - Albedo
		RootParameters[7].InitAsDescriptorTable(1, &DescriptorRanges[2], D3D12_SHADER_VISIBILITY_PIXEL);  // PerObject texture inputs - Normal
		RootParameters[8].InitAsDescriptorTable(1, &DescriptorRanges[3], D3D12_SHADER_VISIBILITY_PIXEL);  // PerObject texture inputs - Roughness
		RootParameters[9].InitAsDescriptorTable(1, &DescriptorRanges[4], D3D12_SHADER_VISIBILITY_PIXEL);  // PerObject texture inputs - Metallic
		RootParameters[10].InitAsDescriptorTable(1, &DescriptorRanges[5], D3D12_SHADER_VISIBILITY_PIXEL); // PerObject texture inputs - AO

		RootParameters[11].InitAsDescriptorTable(1, &DescriptorRanges[6], D3D12_SHADER_VISIBILITY_PIXEL); // Shadow Depth texture

		RootParameters[12].InitAsDescriptorTable(1, &DescriptorRanges[7], D3D12_SHADER_VISIBILITY_PIXEL);  // Sky - Irradiance
		RootParameters[13].InitAsDescriptorTable(1, &DescriptorRanges[8], D3D12_SHADER_VISIBILITY_PIXEL);  // Sky - Environment Map
		RootParameters[14].InitAsDescriptorTable(1, &DescriptorRanges[9], D3D12_SHADER_VISIBILITY_PIXEL);  // Sky - BRDF LUT
		RootParameters[15].InitAsDescriptorTable(1, &DescriptorRanges[10], D3D12_SHADER_VISIBILITY_PIXEL); // Sky - Diffuse

		RootParameters[16].InitAsDescriptorTable(1, &DescriptorRanges[11], D3D12_SHADER_VISIBILITY_PIXEL); // Rasterized Image
		RootParameters[17].InitAsDescriptorTable(1, &DescriptorRanges[12], D3D12_SHADER_VISIBILITY_PIXEL); // Ray Traced Image




		CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc;
		RootSignatureDesc.Init(_countof(RootParameters), RootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		CD3DX12_STATIC_SAMPLER_DESC StaticSamplers[2] = {};
		// Shadow map sampler
		StaticSamplers[0].Init(
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
			0U
		);
		// Scene Sampler
		FLOAT MinLOD = 0.0f, MaxLOD = 9.0f;
		StaticSamplers[1].Init(
			1,
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			m_GraphicsSettings.MipLodBias,
			m_GraphicsSettings.MaxAnisotropy,
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
			MinLOD,
			MaxLOD,
			D3D12_SHADER_VISIBILITY_PIXEL,
			0U
		);
		RootSignatureDesc.NumStaticSamplers = _countof(StaticSamplers);
		RootSignatureDesc.pStaticSamplers = StaticSamplers;

		ComPtr<ID3DBlob> RootSignatureBlob;
		ComPtr<ID3DBlob> ErrorBlob;

		HRESULT hr = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSignatureBlob.GetAddressOf(), ErrorBlob.GetAddressOf());
		ThrowIfFailed(hr, "Failed to serialize root signature for D3D 12 context.");

		hr = m_pDevice->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pDeferredShadingPass_RootSignature));
		ThrowIfFailed(hr, "Failed to create root signature for D3D 12 context.");
	}

	void Direct3D12Context::CreateForwardShadingRootSignature()
	{
		CD3DX12_DESCRIPTOR_RANGE DescriptorRanges[10] = {};
		DescriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);   // PerObject texture inputs - Albedo
		DescriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);   // PerObject texture inputs - Normal
		DescriptorRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);   // PerObject texture inputs - Roughness
		DescriptorRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);   // PerObject texture inputs - Opacity
		DescriptorRanges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);   // PerObject texture inputs - Translucency
		DescriptorRanges[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);   // Shadow Depth texture
		DescriptorRanges[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6);   // Scene Depth texture

		DescriptorRanges[7].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 7);   // Sky - Irradiance
		DescriptorRanges[8].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 8);   // Sky - Environment Map
		DescriptorRanges[9].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 9);   // Sky - BRDF LUT

		CD3DX12_ROOT_PARAMETER RootParameters[14] = {};
		RootParameters[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);	  // Per-Object constant buffer
		RootParameters[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);		  // Per-Frame constant buffer
		RootParameters[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);	      // Light constant buffer
		RootParameters[3].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);		  // Material Additives constant buffer

		RootParameters[4].InitAsDescriptorTable(1, &DescriptorRanges[6], D3D12_SHADER_VISIBILITY_PIXEL);  // PerObject texture inputs - Scene Depth
		RootParameters[5].InitAsDescriptorTable(1, &DescriptorRanges[5], D3D12_SHADER_VISIBILITY_PIXEL);  // PerObject texture inputs - Shadow Depth
		RootParameters[6].InitAsDescriptorTable(1, &DescriptorRanges[0], D3D12_SHADER_VISIBILITY_PIXEL);  // PerObject texture inputs - Albedo
		RootParameters[7].InitAsDescriptorTable(1, &DescriptorRanges[1], D3D12_SHADER_VISIBILITY_PIXEL);  // PerObject texture inputs - Normal
		RootParameters[8].InitAsDescriptorTable(1, &DescriptorRanges[2], D3D12_SHADER_VISIBILITY_PIXEL);  // PerObject texture inputs - Roughness
		RootParameters[9].InitAsDescriptorTable(1, &DescriptorRanges[3], D3D12_SHADER_VISIBILITY_PIXEL);  // PerObject texture inputs - Opacity
		RootParameters[10].InitAsDescriptorTable(1, &DescriptorRanges[4], D3D12_SHADER_VISIBILITY_PIXEL);  // PerObject texture inputs - Translucency

		RootParameters[11].InitAsDescriptorTable(1, &DescriptorRanges[7], D3D12_SHADER_VISIBILITY_PIXEL); // Sky - Irradiance
		RootParameters[12].InitAsDescriptorTable(1, &DescriptorRanges[8], D3D12_SHADER_VISIBILITY_PIXEL); // Sky - Environment Map
		RootParameters[13].InitAsDescriptorTable(1, &DescriptorRanges[9], D3D12_SHADER_VISIBILITY_PIXEL); // Sky - BRDF LUT


		CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc;
		RootSignatureDesc.Init(_countof(RootParameters), RootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		CD3DX12_STATIC_SAMPLER_DESC StaticSamplers[2] = {};
		// Shadow map sampler
		StaticSamplers[0].Init(
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
			0U
		);
		// Scene Sampler
		FLOAT MinLOD = 0.0f;
		FLOAT MaxLOD = 9.0f;
		StaticSamplers[1].Init(
			1,
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			m_GraphicsSettings.MipLodBias,
			m_GraphicsSettings.MaxAnisotropy,
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
			MinLOD,
			MaxLOD,
			D3D12_SHADER_VISIBILITY_PIXEL,
			0U
		);
		RootSignatureDesc.NumStaticSamplers = _countof(StaticSamplers);
		RootSignatureDesc.pStaticSamplers = StaticSamplers;

		ComPtr<ID3DBlob> RootSignatureBlob;
		ComPtr<ID3DBlob> ErrorBlob;

		HRESULT hr = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSignatureBlob.GetAddressOf(), ErrorBlob.GetAddressOf());
		ThrowIfFailed(hr, "Failed to serialize root signature for D3D 12 context.");

		hr = m_pDevice->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pForwardShadingPass_RootSignature));
		ThrowIfFailed(hr, "Failed to create root signature for D3D 12 context.");
	}

	void Direct3D12Context::CreateShadowPassPSO()
	{
		HRESULT hr;

		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pPixelShader;

#ifndef IE_IS_STANDALONE
		LPCWSTR VertexShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Shadow_Pass.vertex.cso";
		LPCWSTR PixelShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Shadow_Pass.pixel.cso";
#else
		LPCWSTR VertexShaderFolder = L"Shadow_Pass.vertex.cso";
		LPCWSTR PixelShaderFolder = L"Shadow_Pass.pixel.cso";
#endif // !IE_IS_STANDALONE

		hr = D3DReadFileToBlob(VertexShaderFolder, &pVertexShader);
		ThrowIfFailed(hr, "Failed to read Vertex Shader for D3D 12 context.");
		hr = D3DReadFileToBlob(PixelShaderFolder, &pPixelShader);
		ThrowIfFailed(hr, "Failed to read Pixel Shader for D3D 12 context.");

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
		PsoDesc.pRootSignature = m_pDeferredShadingPass_RootSignature.Get();
		PsoDesc.DepthStencilState = ShadowDepthStencilDesc;
		PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		PsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		PsoDesc.SampleMask = UINT_MAX;
		PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PsoDesc.DSVFormat = m_ShadowMapFormat;
		PsoDesc.SampleDesc.Count = 1;
		PsoDesc.NumRenderTargets = 0;
		PsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;

		hr = m_pDevice->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&m_pShadowPass_PSO));
		ThrowIfFailed(hr, "Failed to create graphics pipeline state for shadow pass in D3D 12 context.");
		m_pShadowPass_PSO->SetName(L"PSO Shadow Pass");
	}

	void Direct3D12Context::CreateGeometryPassPSO()
	{
		HRESULT hr;

		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pPixelShader;

#ifndef IE_IS_STANDALONE
		LPCWSTR vertexShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Geometry_Pass.vertex.cso";
		LPCWSTR pixelShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Geometry_Pass.pixel.cso";
#else
		LPCWSTR vertexShaderFolder = L"Geometry_Pass.vertex.cso";
		LPCWSTR pixelShaderFolder = L"Geometry_Pass.pixel.cso";
#endif 

		hr = D3DReadFileToBlob(vertexShaderFolder, &pVertexShader);
		ThrowIfFailed(hr, "Failed to read Vertex Shader for D3D 12 context.");
		hr = D3DReadFileToBlob(pixelShaderFolder, &pPixelShader);
		ThrowIfFailed(hr, "Failed to read Pixel Shader for D3D 12 context.");


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
		PsoDesc.pRootSignature = m_pDeferredShadingPass_RootSignature.Get();
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

		hr = m_pDevice->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&m_pGeometryPass_PSO));
		ThrowIfFailed(hr, "Failed to create graphics pipeline state for geometry pass.");
		m_pGeometryPass_PSO->SetName(L"PSO Geometry Pass");
	}

	void Direct3D12Context::CreateSkyPassPSO()
	{
		HRESULT hr;

		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pPixelShader;

#ifndef IE_IS_STANDALONE
		LPCWSTR vertexShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Skybox.vertex.cso";
		LPCWSTR pixelShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Skybox.pixel.cso";
#else
		LPCWSTR vertexShaderFolder = L"Skybox.vertex.cso";
		LPCWSTR pixelShaderFolder = L"Skybox.pixel.cso";
#endif 

		hr = D3DReadFileToBlob(vertexShaderFolder, &pVertexShader);
		ThrowIfFailed(hr, "Failed to compile Vertex Shader for D3D 12 context");
		hr = D3DReadFileToBlob(pixelShaderFolder, &pPixelShader);
		ThrowIfFailed(hr, "Failed to compile Pixel Shader for D3D 12 context");


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
		pipelineDesc.pRootSignature = m_pDeferredShadingPass_RootSignature.Get();
		pipelineDesc.DepthStencilState = depthStencilStateDesc;
		pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pipelineDesc.RasterizerState = rasterizerStateDesc;
		pipelineDesc.SampleMask = UINT_MAX;
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineDesc.NumRenderTargets = 1;
		pipelineDesc.RTVFormats[0] = m_RtvFormat[0];
		pipelineDesc.SampleDesc.Count = 1;
		pipelineDesc.DSVFormat = m_DsvFormat;


		hr = m_pDevice->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&m_pSkyPass_PSO));
		ThrowIfFailed(hr, "Failed to create skybox pipeline state object for .");
		m_pSkyPass_PSO->SetName(L"PSO Sky Pass");
	}

	void Direct3D12Context::CreateTransparencyPassPSO()
	{
		HRESULT hr;

		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pPixelShader;

#ifndef IE_IS_STANDALONE
		LPCWSTR vertexShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Transparency_Pass.vertex.cso";
		LPCWSTR pixelShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Transparency_Pass.pixel.cso";
#else
		LPCWSTR vertexShaderFolder = L"Transparency_Pass.vertex.cso";
		LPCWSTR pixelShaderFolder = L"Transparency_Pass.pixel.cso";
#endif 

		hr = D3DReadFileToBlob(vertexShaderFolder, &pVertexShader);
		ThrowIfFailed(hr, "Failed to read Vertex Shader for D3D 12 context.");
		hr = D3DReadFileToBlob(pixelShaderFolder, &pPixelShader);
		ThrowIfFailed(hr, "Failed to read Pixel Shader for D3D 12 context.");

		D3D12_SHADER_BYTECODE VertexShaderBytecode = {};
		VertexShaderBytecode.BytecodeLength = pVertexShader->GetBufferSize();
		VertexShaderBytecode.pShaderBytecode = pVertexShader->GetBufferPointer();

		D3D12_SHADER_BYTECODE PixelShaderBytecode = {};
		PixelShaderBytecode.BytecodeLength = pPixelShader->GetBufferSize();
		PixelShaderBytecode.pShaderBytecode = pPixelShader->GetBufferPointer();


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

		auto DepthStencilStateDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		DepthStencilStateDesc.DepthEnable = true;
		DepthStencilStateDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		DepthStencilStateDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		auto RasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		RasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;

		auto BlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		BlendDesc.AlphaToCoverageEnable = TRUE;
		BlendDesc.IndependentBlendEnable = TRUE;
		BlendDesc.RenderTarget[0].BlendEnable = true;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
		PsoDesc.VS = VertexShaderBytecode;
		PsoDesc.PS = PixelShaderBytecode;
		PsoDesc.InputLayout.pInputElementDescs = inputLayout;
		PsoDesc.InputLayout.NumElements = _countof(inputLayout);
		PsoDesc.pRootSignature = m_pForwardShadingPass_RootSignature.Get();
		PsoDesc.DepthStencilState = DepthStencilStateDesc;
		PsoDesc.BlendState = BlendDesc;
		PsoDesc.RasterizerState = RasterizerDesc;
		PsoDesc.SampleMask = UINT_MAX;
		PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PsoDesc.NumRenderTargets = 1U;
		PsoDesc.RTVFormats[0] = m_RtvFormat[0];
		//PsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		PsoDesc.DSVFormat = m_DsvFormat;
		PsoDesc.SampleDesc.Count = 1;

		hr = m_pDevice->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&m_pTransparency_PSO));
		ThrowIfFailed(hr, "Failed to create graphics pipeline state for transparency pass.");
		m_pTransparency_PSO->SetName(L"PSO Transparency Pass");
	}

	void Direct3D12Context::CreateLightPassPSO()
	{
		HRESULT hr;

		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pPixelShader;

#ifndef IE_IS_STANDALONE
		LPCWSTR vertexShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Light_Pass.vertex.cso";
		LPCWSTR pixelShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Light_Pass.pixel.cso";
#else
		LPCWSTR vertexShaderFolder = L"Light_Pass.vertex.cso";
		LPCWSTR pixelShaderFolder = L"Light_Pass.pixel.cso";
#endif 

		hr = D3DReadFileToBlob(vertexShaderFolder, &pVertexShader);
		ThrowIfFailed(hr, "Failed to compile Vertex Shader for D3D 12 context.");
		hr = D3DReadFileToBlob(pixelShaderFolder, &pPixelShader);
		ThrowIfFailed(hr, "Failed to compile Pixel Shader for D3D 12 context.");


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
		descPipelineState.pRootSignature = m_pDeferredShadingPass_RootSignature.Get();
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

		hr = m_pDevice->CreateGraphicsPipelineState(&descPipelineState, IID_PPV_ARGS(&m_pLightingPass_PSO));
		ThrowIfFailed(hr, "Failed to create graphics pipeline state for lighting pass.");
		m_pLightingPass_PSO->SetName(L"PSO Light Pass");
	}

	void Direct3D12Context::CreatePostEffectsPassPSO()
	{
		HRESULT hr;

		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pPixelShader;
		//IE_BUILD_DIR
#ifndef IE_IS_STANDALONE
		LPCWSTR vertexShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/PostFx.vertex.cso";
		LPCWSTR pixelShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/PostFx.pixel.cso";
#else
		LPCWSTR vertexShaderFolder = L"PostFx.vertex.cso";
		LPCWSTR pixelShaderFolder = L"PostFx.pixel.cso";
#endif 

		hr = D3DReadFileToBlob(vertexShaderFolder, &pVertexShader);
		ThrowIfFailed(hr, "Failed to compile Vertex Shader for D3D 12 context.");
		hr = D3DReadFileToBlob(pixelShaderFolder, &pPixelShader);
		ThrowIfFailed(hr, "Failed to compile Pixel Shader for D3D 12 context.");


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
		descPipelineState.pRootSignature = m_pDeferredShadingPass_RootSignature.Get();
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

		hr = m_pDevice->CreateGraphicsPipelineState(&descPipelineState, IID_PPV_ARGS(&m_pPostFxPass_PSO));
		ThrowIfFailed(hr, "Failed to create graphics pipeline state for Post-Fx pass in D3D 12 context.");
		m_pPostFxPass_PSO->SetName(L"PSO PostFx Pass");
	}

	void Direct3D12Context::CreateCommandAllocators()
	{
		HRESULT hr;

		// Scene Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pScenePass_CommandAllocators[i]));
				ThrowIfFailed(hr, "Failed to Scene Pass Create Command Allocator for D3D 12 context");
				m_pScenePass_CommandAllocators[i]->SetName(L"Scene Pass Command Allocator");
			}

			hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pScenePass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pScenePass_CommandList));
			ThrowIfFailed(hr, "Failed to Scene Pass Create Command List for D3D 12 context");
			m_pScenePass_CommandList->SetName(L"Scene Pass Command List");

			// Dont close the Scene Pass command list yet. 
			// We'll use it for application initialization.
		}

		// Shadow Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pShadowPass_CommandAllocators[i]));
				ThrowIfFailed(hr, "Failed to Create Command Allocator for D3D 12 context");
				m_pShadowPass_CommandAllocators[i]->SetName(L"Shadow Pass Command Allocator");
			}

			hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pShadowPass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pShadowPass_CommandList));
			ThrowIfFailed(hr, "Failed to Create Shadow Pass Command List for D3D 12 context");
			m_pShadowPass_CommandList->SetName(L"Shadow Pass Command List");

			m_pShadowPass_CommandList->Close();
		}

		// Transprency Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pTransparencyPass_CommandAllocators[i]));
				ThrowIfFailed(hr, "Failed to Create Command Allocator for D3D 12 context");
				m_pTransparencyPass_CommandAllocators[i]->SetName(L"Transparency Pass Command Allocator");
			}

			hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pTransparencyPass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pTransparencyPass_CommandList));
			ThrowIfFailed(hr, "Failed to Create Transparency Pass Command List for D3D 12 context");
			m_pTransparencyPass_CommandList->SetName(L"Transparency Pass Command List");

			m_pTransparencyPass_CommandList->Close();
		}

		// Post-Process Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pPostEffectsPass_CommandAllocators[i]));
				ThrowIfFailed(hr, "Failed to Create Command Allocator for D3D 12 context");
				m_pPostEffectsPass_CommandAllocators[i]->SetName(L"Post-Process Pass Command Allocator");
			}

			hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pPostEffectsPass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pPostEffectsPass_CommandList));
			ThrowIfFailed(hr, "Failed to Create Post-Process Pass Command List for D3D 12 context");
			m_pPostEffectsPass_CommandList->SetName(L"Post-Process Pass Command List");

			m_pPostEffectsPass_CommandList->Close();
		}

		// Ray Trace Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pRayTracePass_CommandAllocators[i]));
				ThrowIfFailed(hr, "Failed to Create Command Allocator for D3D 12 context");
				m_pRayTracePass_CommandAllocators[i]->SetName(L"Post-Process Pass Command Allocator");
			}

			hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pRayTracePass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pRayTracePass_CommandList));
			ThrowIfFailed(hr, "Failed to Create Post-Process Pass Command List for D3D 12 context");
			m_pRayTracePass_CommandList->SetName(L"Post-Process Pass Command List");

			// Dont close the Ray Trace Pass command list yet. 
			// We'll use it for pipeline initialization.
		}

		ID3D12CommandList* ppCommandLists[] = {
			m_pShadowPass_CommandList.Get(),
			m_pTransparencyPass_CommandList.Get(),
			m_pPostEffectsPass_CommandList.Get(),
		};
		m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		m_FenceValues[m_FrameIndex]++;
		ThrowIfFailed(m_pCommandQueue->Signal(m_pFence.Get(), m_FenceValues[m_FrameIndex]), "Failed to signal command queue for command list initialization for D3D 12 context.");
		WaitForGPU();
	}

	void Direct3D12Context::CreateFenceEvent()
	{
		HRESULT hr;
		hr = m_pDevice->CreateFence(m_FenceValues[m_FrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
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
			hr = m_pDevice->CreateCommittedResource(
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
			hr = m_pDevice->CreateCommittedResource(
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
			hr = m_pDevice->CreateCommittedResource(
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
			hr = m_pDevice->CreateCommittedResource(
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
			hr = m_pDevice->CreateCommittedResource(
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
		m_pScenePass_CommandList->Close();
		m_pRayTracePass_CommandList->Close();

		ID3D12CommandList* ppCommandLists[] = {
			m_pScenePass_CommandList.Get(),
			m_pRayTracePass_CommandList.Get()
		};
		m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		m_FenceValues[m_FrameIndex]++;
		HRESULT hr = m_pCommandQueue->Signal(m_pFence.Get(), m_FenceValues[m_FrameIndex]);
		if (FAILED(hr)) {
			IE_CORE_WARN("Command queue failed to signal.");
		}
		m_RTHelper.OnPostInit();

		WaitForGPU();
	}

	void Direct3D12Context::CreateCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		HRESULT hr = m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCommandQueue));
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

		if (m_IsRayTraceSupported) {
			ComPtr<ID3D12Device5> TempDevice;
			HRESULT hr = D3D12CreateDevice(m_pAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&TempDevice));
			ThrowIfFailed(hr, "Failed to create logical device for ray tracing.");
			m_pDevice = TempDevice.Detach();
		}
		else {
			HRESULT hr = D3D12CreateDevice(m_pAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_pDevice));
			ThrowIfFailed(hr, "Failed to create logical device.");
		}
	}

	void Direct3D12Context::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
	{
		ComPtr<IDXGIAdapter1> pAdapter;
		*ppAdapter = nullptr;
		UINT currentVideoCardMemory = 0;
		DXGI_ADAPTER_DESC1 Desc;

		auto CheckRayTracingSupport = [](ID3D12Device* pDevice) {

			D3D12_FEATURE_DATA_D3D12_OPTIONS5 Options5 = {};
			ThrowIfFailed(pDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &Options5, sizeof(Options5)), "Failed to query feature support for ray trace with device.");
			if (Options5.RaytracingTier < D3D12_RAYTRACING_TIER_1_0) {
				IE_CORE_WARN("Ray tracing not supported on this device.");
				return false;
			}
			return true;
		};

		for (UINT AdapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(AdapterIndex, &pAdapter); ++AdapterIndex)
		{
			Desc = {};
			pAdapter->GetDesc1(&Desc);

			// Make sure we get the video card that is not a software adapter
			// and it has the most video memory
			if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE || Desc.DedicatedVideoMemory < currentVideoCardMemory) continue;

			// Check if we can support ray tracing with the device
			if (m_IsRayTraceEnabled) {

				ComPtr<ID3D12Device5> TempDevice;
				if (SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device5), &TempDevice))) {
					if (CheckRayTracingSupport(TempDevice.Get())) {

						currentVideoCardMemory = static_cast<UINT>(Desc.DedicatedVideoMemory);
						if (*ppAdapter != nullptr) {
							(*ppAdapter)->Release();
						}
						*ppAdapter = pAdapter.Detach();

						m_IsRayTraceSupported = true;

						IE_CORE_WARN("Found suitable Direct3D 12 graphics hardware that can support ray tracing: {0}", StringHelper::WideToString(std::wstring{ Desc.Description }));
						continue;
					}
				}
			}

			// If we cannot support ray tracing, just see if D3D 12 is supported and create a default device
			if (SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr))) {

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
		IE_CORE_WARN("\"{0}\" selected as Direct3D 12 graphics hardware.", StringHelper::WideToString(Desc.Description));
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
				m_pDevice->CreateRenderTargetView(m_pRenderTargets[i].Get(), nullptr, m_rtvHeap.hCPU(i));
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