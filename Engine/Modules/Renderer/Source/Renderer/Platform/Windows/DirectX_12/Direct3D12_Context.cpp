#include <Renderer_pch.h>

#include "Direct3D12_Context.h"

#include "Insight/Core/Application.h"
#include "Platform/Windows/Windows_Window.h"
#include "Insight/Runtime/Archetypes/APlayer_Character.h"
#include "Insight/Systems/Managers/Geometry_Manager.h"

#include "Insight/Rendering/APost_Fx.h"
#include "Insight/Rendering/ASky_Light.h"
#include "Insight/Rendering/ASky_Sphere.h"
#include "Insight/Rendering/Lighting/ASpot_Light.h"
#include "Insight/Rendering/Lighting/APoint_Light.h"

#include "Platform/Windows/DirectX_12/Geometry/D3D12_Vertex_Buffer.h"
#include "Platform/Windows/DirectX_12/Geometry/D3D12_Index_Buffer.h"
#include "Platform/Windows/DirectX_12/Geometry/D3D12_Sphere_Renderer.h"

#define BLOOM_ENABLED 0

namespace Insight {




	Direct3D12Context::Direct3D12Context(WindowsWindow* WindowHandle)
		: m_pWindowRef(WindowHandle),
		Renderer(WindowHandle->GetWidth(), WindowHandle->GetHeight(), false)
	{
		IE_CORE_ASSERT(WindowHandle != nullptr, "Window handle is NULL, cannot initialize D3D 12 context with NULL window handle.");
		m_AspectRatio = static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight);
	}

	Direct3D12Context::~Direct3D12Context()
	{
		InternalCleanup();
	}

	void Direct3D12Context::InternalCleanup()
	{
		// Wait until GPU is done consuming all data
		// before closing all handles and releasing resources
		m_d3dDeviceResources.WaitForGPU();

		if (m_GraphicsSettings.RayTraceEnabled) m_RTHelper.OnDestroy();

		if (!m_AllowTearing) {
			m_d3dDeviceResources.GetSwapChain().SetFullscreenState(false, NULL);
		}

		m_d3dDeviceResources.CleanUp();
	}

	bool Direct3D12Context::Init_Impl()
	{
		IE_CORE_INFO("Renderer: D3D 12");

		try 
		{
			m_d3dDeviceResources.Init(this);
			
			m_FrameResources.Init(&m_d3dDeviceResources.GetDeviceContext());
			HRESULT hr = m_cbvsrvHeap.Create(&m_d3dDeviceResources.GetDeviceContext(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 70, true);
			ThrowIfFailed(hr, "Failed to create descriptor heap for shader visible resources.");

			CreateCommandAllocators();

			//CreateSRVs();

			if (m_GraphicsSettings.RayTraceEnabled)
				m_RTHelper.OnInit(this);

			PIXBeginEvent(&m_d3dDeviceResources.GetGraphicsCommandQueue(), 0, L"D3D12 Context Setup");
			{
				//CreateScreenQuad();
				CreateViewport();
				CreateScissorRect();

				// Load Pipelines
				{
					CreateDeferredShadingRS();
					CreateForwardShadingRS();
					CreateBloomPassRS();
					CreateDebugScreenQuadRS();
					
					{
						// Create the Geometry Pass and push it to the render stack.
						m_GeometryPass.Create(this, &m_cbvsrvHeap, m_pScenePass_CommandList.Get(), m_pDeferredShadingPass_RS.Get());
						m_RenderPassStack.PushPass(&m_GeometryPass);
						
						// Create the Light Pass and push it to the render stack.
						m_LightPass.Create(this, &m_cbvsrvHeap, m_pScenePass_CommandList.Get(), m_pDeferredShadingPass_RS.Get());
						uint8_t NumGBuffers = m_GeometryPass.GetNumGBuffers();
						for (uint8_t i = 0; i < NumGBuffers; ++i)
							m_LightPass.SetRenderTargetTextureRef(m_GeometryPass.GetGBufferRenderTargetTexture(i));
						m_LightPass.SetSceneDepthTextureRef(m_GeometryPass.GetSceneDepthTexture());
						m_RenderPassStack.PushPass(&m_LightPass);
						
						// Create the Post Process Composite Pass ad push it to the render stack.
						m_PostProcessCompositePass.Create(this, &m_cbvsrvHeap, m_pPostEffectsPass_CommandList.Get(), m_pDeferredShadingPass_RS.Get());
						m_PostProcessCompositePass.SetSceneDepthTextureRef(m_GeometryPass.GetSceneDepthTexture());
						m_RenderPassStack.PushPassOverlay(&m_PostProcessCompositePass);



					}

					
					//LoadPipelines();
				}

				// Create Render Targets and Depth Stencils
				{
					//CreateDSVs();
					//CreateRTVs();
					CreateSwapChainRTVDescriptorHeap();
				}

			}
			PIXEndEvent(&m_d3dDeviceResources.GetGraphicsCommandQueue());
		}
		catch (COMException& Ex) 
		{
			m_pWindowRef->CreateMessageBox(Ex.what(), L"Fatal Error");
			return false;
		}
		return true;
	}

	void Direct3D12Context::Destroy_Impl()
	{
		InternalCleanup();
	}

	bool Direct3D12Context::PostInit_Impl()
	{
		CloseCommandListAndSignalCommandQueue();

		return true;
	}

	void Direct3D12Context::OnUpdate_Impl(const float DeltaMs)
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		static float WorldSecond = 0.0f;
		WorldSecond += DeltaMs;

		XMVECTOR InvMatDeterminent;

		// Send Per-Frame Data to GPU
		m_FrameResources.m_CBPerFrame.Data.View = m_pWorldCameraRef->GetViewMatrix();
		m_FrameResources.m_CBPerFrame.Data.Projection = m_pWorldCameraRef->GetProjectionMatrix();
		m_FrameResources.m_CBPerFrame.Data.InverseView = XMMatrixInverse(&InvMatDeterminent, m_pWorldCameraRef->GetViewMatrix());
		m_FrameResources.m_CBPerFrame.Data.InverseProjection = XMMatrixInverse(&InvMatDeterminent, m_pWorldCameraRef->GetProjectionMatrix());
		m_FrameResources.m_CBPerFrame.Data.CameraPosition = m_pWorldCameraRef->GetPosition();
		m_FrameResources.m_CBPerFrame.Data.DeltaMs = DeltaMs;
		m_FrameResources.m_CBPerFrame.Data.WorldTime = WorldSecond;
		m_FrameResources.m_CBPerFrame.Data.CameraNearZ = m_pWorldCameraRef->GetNearZ();
		m_FrameResources.m_CBPerFrame.Data.CameraFarZ = m_pWorldCameraRef->GetFarZ();
		m_FrameResources.m_CBPerFrame.Data.CameraExposure = m_pWorldCameraRef->GetExposure();
		m_FrameResources.m_CBPerFrame.Data.NumPointLights = (float)m_PointLights.size();
		m_FrameResources.m_CBPerFrame.Data.NumDirectionalLights = (m_pWorldDirectionalLight != nullptr) ? 1.0f : 0.0f;
		m_FrameResources.m_CBPerFrame.Data.RayTraceEnabled = (float)m_GraphicsSettings.RayTraceEnabled;
		m_FrameResources.m_CBPerFrame.Data.NumSpotLights = (float)m_SpotLights.size();
		m_FrameResources.m_CBPerFrame.Data.ScreenSize.x = (float)m_WindowWidth;
		m_FrameResources.m_CBPerFrame.Data.ScreenSize.y = (float)m_WindowHeight;
		m_FrameResources.m_CBPerFrame.SubmitToGPU();

		if (m_GraphicsSettings.RayTraceEnabled) m_RTHelper.UpdateCBVs();


		// Send Point Lights to GPU
		for (int i = 0; i < m_PointLights.size(); i++)
			m_FrameResources.m_CBLights.Data.PointLights[i] = m_PointLights[i]->GetConstantBuffer();

		// Send Directionl Light to GPU
		if (m_pWorldDirectionalLight)
			m_FrameResources.m_CBLights.Data.DirectionalLight = m_pWorldDirectionalLight->GetConstantBuffer();

		// Send Spot Lights to GPU
		for (int i = 0; i < m_SpotLights.size(); i++)
			m_FrameResources.m_CBLights.Data.SpotLights[i] = m_SpotLights[i]->GetConstantBuffer();

		m_FrameResources.m_CBLights.SubmitToGPU();

		// Send Post-Fx data to GPU
		if (m_pPostFx)
		{
			m_FrameResources.m_CBPostProcessParams.Data = m_pPostFx->GetConstantBuffer();
			m_FrameResources.m_CBPostProcessParams.SubmitToGPU();
		}
	}

	void Direct3D12Context::OnPreFrameRender_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		// Reset Command Allocators
		{
			ThrowIfFailed(m_pScenePass_CommandAllocators[IE_D3D12_FrameIndex]->Reset(),
				"Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Scene Pass");

			//ThrowIfFailed(m_pShadowPass_CommandAllocators[IE_D3D12_FrameIndex]->Reset(),
			//	"Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Shadow Pass");

			//ThrowIfFailed(m_pTransparencyPass_CommandAllocators[IE_D3D12_FrameIndex]->Reset(),
			//	"Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Transparency Pass");

			ThrowIfFailed(m_pPostEffectsPass_CommandAllocators[IE_D3D12_FrameIndex]->Reset(),
				"Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Post-Process Pass");
#if BLOOM_ENABLED
			ThrowIfFailed(m_pDownSample_CommandAllocators[IE_D3D12_FrameIndex]->Reset(),
				"Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Post-Process Pass");
#endif // BLOOM_ENABLED

			//if (m_GraphicsSettings.RayTraceEnabled) {
			//	ThrowIfFailed(m_pRayTracePass_CommandAllocators[IE_D3D12_FrameIndex]->Reset(),
			//		"Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Ray Trace Pass");
			//}
		}

		// Reset Command Lists
		{
			ThrowIfFailed(m_pScenePass_CommandList->Reset(m_pScenePass_CommandAllocators[IE_D3D12_FrameIndex].Get(), m_pGeometryPass_PSO.Get()),
				"Failed to reset command list in Direct3D12Context::OnPreFrameRender for Scene Pass");

			//ThrowIfFailed(m_pShadowPass_CommandList->Reset(m_pShadowPass_CommandAllocators[IE_D3D12_FrameIndex].Get(), m_pShadowPass_PSO.Get()),
			//	"Failed to reset command list in Direct3D12Context::OnPreFrameRender for Shadow Pass");

			//ThrowIfFailed(m_pTransparencyPass_CommandList->Reset(m_pTransparencyPass_CommandAllocators[IE_D3D12_FrameIndex].Get(), m_pTransparency_PSO.Get()),
			//	"Failed to reset command list in Direct3D12Context::OnPreFrameRender for Transparency Pass");

			ThrowIfFailed(m_pPostEffectsPass_CommandList->Reset(m_pPostEffectsPass_CommandAllocators[IE_D3D12_FrameIndex].Get(), m_pPostFxPass_PSO.Get()),
				"Failed to reset command list in Direct3D12Context::OnPreFrameRender for Transparency Pass");

#if BLOOM_ENABLED
			ThrowIfFailed(m_pDownSample_CommandList->Reset(m_pDownSample_CommandAllocators[IE_D3D12_FrameIndex].Get(), m_pThresholdDownSample_PSO.Get()),
				"Failed to reset command list in Direct3D12Context::OnPreFrameRender for Transparency Pass");
#endif
			//if (m_GraphicsSettings.RayTraceEnabled) {
			//	ThrowIfFailed(m_pRayTracePass_CommandList->Reset(m_pRayTracePass_CommandAllocators[IE_D3D12_FrameIndex].Get(), nullptr),
			//		"Failed to reset command list in Direct3D12Context::OnPreFrameRender for Ray Trace Pass");
			//}
		}

		// Prepare the Render Target for this Frame
		{
			ResourceBarrier(m_pScenePass_CommandList.Get(), GetSwapChainRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			const float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			m_pScenePass_CommandList->ClearRenderTargetView(GetSwapChainRTV(), ClearColor, 0, nullptr);
		}

		// Reset Scene Pass
		PIXBeginEvent(m_pScenePass_CommandList.Get(), 0, L"Resetting Scene Pass Command List");
		{
			m_pScenePass_CommandList->RSSetScissorRects(1, &m_d3dDeviceResources.GetClientScissorRect());
			m_pScenePass_CommandList->RSSetViewports(1, &m_d3dDeviceResources.GetClientViewPort());
		}
		PIXEndEvent(m_pScenePass_CommandList.Get());

		// Reset Shadow Pass
		/*PIXBeginEvent(m_pShadowPass_CommandList.Get(), 0, L"Resetting Shadow Pass Command List");
		{
			m_pShadowPass_CommandList->RSSetScissorRects(1, &m_ShadowPass_ScissorRect);
			m_pShadowPass_CommandList->RSSetViewports(1, &m_ShadowPass_ViewPort);
			m_pShadowPass_CommandList->OMSetRenderTargets(0, nullptr, FALSE, &m_dsvHeap.hCPU(1));
		}
		PIXEndEvent(m_pShadowPass_CommandList.Get());*/
	}

	void Direct3D12Context::OnRender_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;


		// Gather the geometry in the world and send it to the GPU.
		GeometryManager::GatherGeometry();

		/*m_CBPerFrame.SetAsGraphicsRootConstantBufferView(m_pPostEffectsPass_CommandList.Get(), 2);
		m_CBPerFrame.SetAsGraphicsRootConstantBufferView(m_pScenePass_CommandList.Get(), 1);
		m_CBPerFrame.SetAsGraphicsRootConstantBufferView(m_pPostEffectsPass_CommandList.Get(), 1);

		m_CBLights.SetAsGraphicsRootConstantBufferView(m_pScenePass_CommandList.Get(), 2);
		m_CBLights.SetAsGraphicsRootConstantBufferView(m_pShadowPass_CommandList.Get(), 2);
		m_CBLights.SetAsGraphicsRootConstantBufferView(m_pTransparencyPass_CommandList.Get(), 2);

		m_CBPostFx.SetAsGraphicsRootConstantBufferView(m_pPostEffectsPass_CommandList.Get(), 3);*/

		// Traverse the render stack and draw the scene.
		for (RenderPass* Pass : m_RenderPassStack)
		{
			Pass->Render(&m_FrameResources);
		}

		return;


		// Render Shadows.
		m_pActiveCommandList = m_pShadowPass_CommandList;
		BindShadowPass();

		// Render Scene.
		m_pActiveCommandList = m_pScenePass_CommandList;
		BindGeometryPass();

		// Transparency Forward Pass.
		m_pActiveCommandList = m_pTransparencyPass_CommandList;
		BindTransparencyPass();

		if (m_GraphicsSettings.RayTraceEnabled) {
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
			m_pShadowPass_CommandList->SetGraphicsRootSignature(m_pDeferredShadingPass_RS.Get());

			ResourceBarrier(m_pShadowPass_CommandList.Get(), m_pShadowDepthTexture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);

			m_pShadowPass_CommandList->ClearDepthStencilView(m_dsvHeap.hCPU(1), D3D12_CLEAR_FLAG_DEPTH, m_DepthClearValue, 0xFF, 0, nullptr);
			m_pShadowPass_CommandList->OMSetRenderTargets(0, nullptr, FALSE, &m_dsvHeap.hCPU(1));

			m_pShadowPass_CommandList->RSSetScissorRects(1, &m_ShadowPass_ScissorRect);
			m_pShadowPass_CommandList->RSSetViewports(1, &m_ShadowPass_ViewPort);

			m_pShadowPass_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_FrameResources.m_CBPerFrame.SetAsGraphicsRootConstantBufferView(m_pShadowPass_CommandList.Get(), 1);
			m_FrameResources.m_CBLights.SetAsGraphicsRootConstantBufferView(m_pShadowPass_CommandList.Get(), 2);

			// TODO Shadow pass logic here put this on another thread
			GeometryManager::Render(RenderPassType::RenderPassType_Shadow);
			ResourceBarrier(m_pShadowPass_CommandList.Get(), m_pShadowDepthTexture.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}
		PIXEndEvent(m_pShadowPass_CommandList.Get());
	}

	void Direct3D12Context::BindGeometryPass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;
		
		PIXBeginEvent(m_pScenePass_CommandList.Get(), 0, L"Rendering Geometry Pass");
		{
			ID3D12DescriptorHeap* ppHeaps[] = { m_cbvsrvHeap.pDH.Get() };
			m_pScenePass_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			for (int i = 0; i < m_NumRTVs; i++) {
				m_pScenePass_CommandList->ClearRenderTargetView(m_rtvHeap.hCPU(i), m_ScreenClearColor, 0, nullptr);
			}

			m_pScenePass_CommandList->ClearDepthStencilView(m_dsvHeap.hCPU(0), D3D12_CLEAR_FLAG_DEPTH, m_DepthClearValue, 0xFF, 0, nullptr);

			m_pScenePass_CommandList->OMSetRenderTargets(m_NumRTVs, &m_rtvHeap.hCPUHeapStart, TRUE, &m_dsvHeap.hCPU(0));
			m_pScenePass_CommandList->SetGraphicsRootSignature(m_pDeferredShadingPass_RS.Get());

			{
				m_pScenePass_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				m_FrameResources.m_CBLights.SetAsGraphicsRootConstantBufferView(m_pScenePass_CommandList.Get(), 2);
				m_FrameResources.m_CBPerFrame.SetAsGraphicsRootConstantBufferView(m_pScenePass_CommandList.Get(), 1);
			}

			GeometryManager::Render(RenderPassType::RenderPassType_Scene);
		}
		PIXEndEvent(m_pScenePass_CommandList.Get());
	}

	void Direct3D12Context::OnMidFrameRender_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		BindLightingPass();
		
		BindSkyPass();

#if BLOOM_ENABLED
		BlurBloomBuffer();
#endif // BLOOM_ENABLED

		BindPostFxPass();

		//DrawDebugScreenQuad();

	}

	void Direct3D12Context::BindRayTracePass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		PIXBeginEvent(m_pRayTracePass_CommandList.Get(), 0, L"Rendering Ray Trace Pass");
		{
			m_RTHelper.SetCommonPipeline();
			m_RTHelper.TraceScene();

			ResourceBarrier(m_pRayTracePass_CommandList.Get(), m_RayTraceOutput_SRV.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
			m_pRayTracePass_CommandList->CopyResource(m_RayTraceOutput_SRV.Get(), m_RTHelper.GetOutputBuffer());
			ResourceBarrier(m_pRayTracePass_CommandList.Get(), m_RayTraceOutput_SRV.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}
		PIXEndEvent(m_pRayTracePass_CommandList.Get());
	}

	void Direct3D12Context::BindLightingPass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		PIXBeginEvent(m_pScenePass_CommandList.Get(), 0, L"Rendering Light Pass");
		{
			D3D12_CPU_DESCRIPTOR_HANDLE RTVHandles[] = { m_rtvHeap.hCPU(4), m_rtvHeap.hCPU(5) };
			m_pScenePass_CommandList->OMSetRenderTargets(_countof(RTVHandles), RTVHandles, FALSE, nullptr);

			if (m_pSkyLight) {
				m_pSkyLight->BindCubeMaps(true);
			}

			for (uint8_t i = 0; i < m_NumRTVs - 2; ++i)
				ResourceBarrier(m_pScenePass_CommandList.Get(), m_pRenderTargetTextures[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			ResourceBarrier(m_pScenePass_CommandList.Get(), m_pSceneDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ);
			m_pScenePass_CommandList->SetPipelineState(m_pLightingPass_PSO.Get());

			m_pScenePass_CommandList->SetGraphicsRootDescriptorTable(17, m_cbvsrvHeap.hGPU(6)); // Ray Trace Result
			m_pScenePass_CommandList->SetGraphicsRootDescriptorTable(11, m_cbvsrvHeap.hGPU(7)); // Shadow Depth
			m_pScenePass_CommandList->SetGraphicsRootDescriptorTable(5, m_cbvsrvHeap.hGPU(0));  // G Buffer

			m_ScreenQuad.OnRender(m_pScenePass_CommandList);

			for (uint8_t i = 4; i < m_NumRTVs; ++i)
				ResourceBarrier(m_pScenePass_CommandList.Get(), m_pRenderTargetTextures[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}
		PIXEndEvent(m_pScenePass_CommandList.Get());
	}

	void Direct3D12Context::BindSkyPass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		ResourceBarrier(m_pScenePass_CommandList.Get(), m_pSceneDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		PIXBeginEvent(m_pScenePass_CommandList.Get(), 0, L"Rendering Sky Pass");
		{
			if (m_pSkySphere) {

				ResourceBarrier(m_pScenePass_CommandList.Get(), m_pRenderTargetTextures[4].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
				m_pScenePass_CommandList->OMSetRenderTargets(1, &m_rtvHeap.hCPU(4), TRUE, &m_dsvHeap.hCPU(0));

				m_pScenePass_CommandList->SetPipelineState(m_pSkyPass_PSO.Get());

				m_pSkySphere->RenderSky();
				ResourceBarrier(m_pScenePass_CommandList.Get(), m_pRenderTargetTextures[4].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
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
			m_pTransparencyPass_CommandList->SetGraphicsRootSignature(m_pForwardShadingPass_RS.Get());

			m_pTransparencyPass_CommandList->RSSetScissorRects(1, &m_d3dDeviceResources.GetClientScissorRect());
			m_pTransparencyPass_CommandList->RSSetViewports(1, &m_d3dDeviceResources.GetClientViewPort());
			m_pTransparencyPass_CommandList->OMSetRenderTargets(1, &m_rtvHeap.hCPU(4), TRUE, &m_dsvHeap.hCPU(0));

			m_pTransparencyPass_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_FrameResources.m_CBPerFrame.SetAsGraphicsRootConstantBufferView(m_pTransparencyPass_CommandList.Get(), 1);
			m_FrameResources.m_CBLights.SetAsGraphicsRootConstantBufferView(m_pTransparencyPass_CommandList.Get(), 2);
			//m_pTransparencyPass_CommandList->SetGraphicsRootDescriptorTable(4, m_cbvsrvHeap.hGPU(4));

			GeometryManager::Render(RenderPassType::RenderPassType_Transparency);
		}
		PIXEndEvent(m_pTransparencyPass_CommandList.Get());
	}

	void Direct3D12Context::BindPostFxPass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		PIXBeginEvent(m_pPostEffectsPass_CommandList.Get(), 0, L"Rendering Post-Process Pass");
		{
			ID3D12DescriptorHeap* ppHeaps[] = { m_cbvsrvHeap.pDH.Get() };
			m_pPostEffectsPass_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			m_pPostEffectsPass_CommandList->OMSetRenderTargets(1, &GetSwapChainRTV(), TRUE, nullptr);
			m_pPostEffectsPass_CommandList->RSSetScissorRects(1, &m_d3dDeviceResources.GetClientScissorRect());
			m_pPostEffectsPass_CommandList->RSSetViewports(1, &m_d3dDeviceResources.GetClientViewPort());

			ResourceBarrier(m_pPostEffectsPass_CommandList.Get(), m_pSceneDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ);
			m_pPostEffectsPass_CommandList->SetGraphicsRootSignature(m_pDeferredShadingPass_RS.Get());

			m_pPostEffectsPass_CommandList->SetPipelineState(m_pPostFxPass_PSO.Get());
			m_pPostEffectsPass_CommandList->SetGraphicsRootDescriptorTable(16, m_cbvsrvHeap.hGPU(5)); // Light Pass result
			m_pPostEffectsPass_CommandList->SetGraphicsRootDescriptorTable(18, m_cbvsrvHeap.hGPU(9)); // Bloom Pass result

			m_FrameResources.m_CBPerFrame.SetAsGraphicsRootConstantBufferView(m_pPostEffectsPass_CommandList.Get(), 1);
			m_FrameResources.m_CBPostProcessParams.SetAsGraphicsRootConstantBufferView(m_pPostEffectsPass_CommandList.Get(), 3);


			m_ScreenQuad.OnRender(m_pPostEffectsPass_CommandList);
		}
		PIXEndEvent(m_pPostEffectsPass_CommandList.Get());
	}

	void Direct3D12Context::DrawDebugScreenQuad()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		PIXBeginEvent(m_pPostEffectsPass_CommandList.Get(), 0, L"Rendering Debug Screen Quad Pass");
		{
			ID3D12DescriptorHeap* ppHeaps[] = { m_cbvsrvHeap.pDH.Get() };
			m_pPostEffectsPass_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			m_pPostEffectsPass_CommandList->OMSetRenderTargets(1, &GetSwapChainRTV(), TRUE, nullptr);

			m_pPostEffectsPass_CommandList->RSSetScissorRects(1, &m_d3dDeviceResources.GetClientScissorRect());
			m_pPostEffectsPass_CommandList->RSSetViewports(1, &m_d3dDeviceResources.GetClientViewPort());

			m_pPostEffectsPass_CommandList->SetPipelineState(m_pDebugScreenQuad_PSO.Get());
			m_pPostEffectsPass_CommandList->SetGraphicsRootSignature(m_pDebugScreenQuad_RS.Get());

			m_FrameResources.m_CBLights.SetAsGraphicsRootConstantBufferView(m_pPostEffectsPass_CommandList.Get(), 1); // Set the light buffer
			m_FrameResources.m_CBPerFrame.SetAsGraphicsRootConstantBufferView(m_pPostEffectsPass_CommandList.Get(), 2); // Set the per frame buffer
			m_pPostEffectsPass_CommandList->SetGraphicsRootDescriptorTable(0, m_cbvsrvHeap.hGPU(7)); // Debug Texture

			m_DebugScreenQuad.OnRender(m_pPostEffectsPass_CommandList);
		}
		PIXEndEvent(m_pPostEffectsPass_CommandList.Get());
	}

	void Direct3D12Context::BlurBloomBuffer()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		constexpr UINT ThreadsPerPixel = 16U;

		PIXBeginEvent(m_pDownSample_CommandList.Get(), 0, L"Computing Bloom Blur Pass");
		{
			ID3D12DescriptorHeap* ppHeaps[] = { m_cbvsrvHeap.pDH.Get() };
			m_pDownSample_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			// Down sample the buffer
			PIXBeginEvent(m_pDownSample_CommandList.Get(), 0, L"Down-sampling the bloom texture");
			{
				m_pDownSample_CommandList->SetPipelineState(m_pThresholdDownSample_PSO.Get());
				m_pDownSample_CommandList->SetComputeRootSignature(m_pBloomPass_RS.Get());

				m_FrameResources.m_CBDownSampleParams.Data.Threshold = 0.5f;
				m_FrameResources.m_CBDownSampleParams.SubmitToGPU();
				m_FrameResources.m_CBDownSampleParams.SetAsComputeRootConstantBufferView(m_pDownSample_CommandList.Get(), 0);
				m_pDownSample_CommandList->SetComputeRootDescriptorTable(1, m_cbvsrvHeap.hGPU(8));
				m_pDownSample_CommandList->SetComputeRootDescriptorTable(2, m_cbvsrvHeap.hGPU(9));

				m_pDownSample_CommandList->Dispatch(m_WindowWidth / ThreadsPerPixel, m_WindowHeight / ThreadsPerPixel, 1);
			}
			PIXEndEvent(m_pDownSample_CommandList.Get());

			m_pDownSample_CommandList->SetPipelineState(m_pGaussianBlur_PSO.Get());

			// Begin blur, two passes, horizontal then vertical
			// Horizontal Pass
			// ---------------
			PIXBeginEvent(m_pDownSample_CommandList.Get(), 0, L"Blurring bloom texture HORIZONTALLY");
			{
				ResourceBarrier(m_pDownSample_CommandList.Get(), m_pBloomBlurResult_UAV.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
				// Copy the UAV to a shader readable SRV
				ResourceBarrier(m_pDownSample_CommandList.Get(), m_pBloomBlurResult_SRV.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
				m_pDownSample_CommandList->CopyResource(m_pBloomBlurResult_SRV.Get(), m_pBloomBlurResult_UAV.Get());
				ResourceBarrier(m_pDownSample_CommandList.Get(), m_pBloomBlurResult_SRV.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
				ResourceBarrier(m_pDownSample_CommandList.Get(), m_pBloomBlurResult_UAV.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);


				m_pDownSample_CommandList->SetComputeRootDescriptorTable(1, m_cbvsrvHeap.hGPU(10)); // Set the SRV of the down sampled bloom texture
				m_pDownSample_CommandList->SetComputeRootDescriptorTable(2, m_cbvsrvHeap.hGPU(11)); // Set the UAV for the intermediate buffer

				m_FrameResources.m_CBBlurParams.Data.Direction = 0;
				m_FrameResources.m_CBBlurParams.SubmitToGPU();
				m_FrameResources.m_CBBlurParams.SetAsComputeRootConstantBufferView(m_pDownSample_CommandList.Get(), 0);

				m_pDownSample_CommandList->Dispatch(m_WindowWidth / ThreadsPerPixel, m_WindowHeight / ThreadsPerPixel, 1);
			}
			PIXEndEvent(m_pDownSample_CommandList.Get());

			// Vertical Pass
			// -------------
			PIXBeginEvent(m_pDownSample_CommandList.Get(), 0, L"Blurring bloom texture VERTICALLY");
			{
				ResourceBarrier(m_pDownSample_CommandList.Get(), m_pBloomBlurIntermediateBuffer_UAV.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
				// Copy the UAV to a shader readable SRV
				ResourceBarrier(m_pDownSample_CommandList.Get(), m_pBloomBlurIntermediateBuffer_SRV.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
				m_pDownSample_CommandList->CopyResource(m_pBloomBlurIntermediateBuffer_SRV.Get(), m_pBloomBlurIntermediateBuffer_UAV.Get());
				ResourceBarrier(m_pDownSample_CommandList.Get(), m_pBloomBlurIntermediateBuffer_SRV.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
				ResourceBarrier(m_pDownSample_CommandList.Get(), m_pBloomBlurIntermediateBuffer_UAV.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

				m_FrameResources.m_CBBlurParams.Data.Direction = 1;
				m_FrameResources.m_CBBlurParams.SubmitToGPU();
				m_FrameResources.m_CBBlurParams.SetAsComputeRootConstantBufferView(m_pDownSample_CommandList.Get(), 0);

				m_pDownSample_CommandList->SetComputeRootDescriptorTable(1, m_cbvsrvHeap.hGPU(12)); // Set the SRV of the down sampled intermediate bloom texture
				m_pDownSample_CommandList->SetComputeRootDescriptorTable(2, m_cbvsrvHeap.hGPU(9)); // Set the origional UAV for final blur pass

				m_pDownSample_CommandList->Dispatch(m_WindowWidth / ThreadsPerPixel, m_WindowHeight / ThreadsPerPixel, 1);
			}
			PIXEndEvent(m_pDownSample_CommandList.Get());
		}
		PIXEndEvent(m_pDownSample_CommandList.Get());
	}

	void Direct3D12Context::ExecuteDraw_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		// Prepare the buffers for next frame
		{
			for (unsigned int i = 0; i < m_GeometryPass.GetNumGBuffers(); ++i) {
				ResourceBarrier(m_pPostEffectsPass_CommandList.Get(), m_GeometryPass.GetGBufferRenderTargetTexture(i).Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
			}
			ResourceBarrier(m_pPostEffectsPass_CommandList.Get(), m_GeometryPass.GetSceneDepthTexture().Get(), D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		}

		// Prepare render target to be presented on the swap chain
		ResourceBarrier(m_pPostEffectsPass_CommandList.Get(), GetSwapChainRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		//ThrowIfFailed(m_pShadowPass_CommandList->Close(), "Failed to close the command list for D3D 12 context shadow pass.");
		ThrowIfFailed(m_pScenePass_CommandList->Close(), "Failed to close command list for D3D 12 context scene pass.");
		//ThrowIfFailed(m_pTransparencyPass_CommandList->Close(), "Failed to close the command list for D3D 12 context transparency pass.");
		ThrowIfFailed(m_pPostEffectsPass_CommandList->Close(), "Failed to close the command list for D3D 12 context post-process pass.");
#if BLOOM_ENABLED
		ThrowIfFailed(m_pDownSample_CommandList->Close(), "Failed to close command list for D3D 12 context bloom blur pass.");
#endif
		if (m_GraphicsSettings.RayTraceEnabled) {
			ThrowIfFailed(m_pRayTracePass_CommandList->Close(), "Failed to close the command list for D3D 12 context ray trace pass.");
		}


		// Scene Pass
		ID3D12CommandList* ppScenePassLists[] = {
			//m_pShadowPass_CommandList.Get(), // Execute shadow pass first because we'll need the depth textures for the light pass
			//m_pRayTracePass_CommandList.Get(),
			m_pScenePass_CommandList.Get(),
			//m_pTransparencyPass_CommandList.Get(),
			m_pPostEffectsPass_CommandList.Get(),

		};
		m_d3dDeviceResources.GetGraphicsCommandQueue().ExecuteCommandLists(_countof(ppScenePassLists), ppScenePassLists);

		// Bloom Compute
#if BLOOM_ENABLED
		ID3D12CommandList* ppComputeLists[] = {
			m_pDownSample_CommandList.Get(),
		};
		m_d3dDeviceResources.GetComputeCommandQueue().ExecuteCommandLists(_countof(ppComputeLists), ppComputeLists);

		// Post Process Pass
		ID3D12CommandList* ppPostFxPassLists[] = {
			m_pPostEffectsPass_CommandList.Get(),
		};
		m_d3dDeviceResources.GetGraphicsCommandQueue().ExecuteCommandLists(_countof(ppPostFxPassLists), ppPostFxPassLists);

#endif // BLOOM_ENABLED

		m_d3dDeviceResources.WaitForGPU();
	}

	void Direct3D12Context::SwapBuffers_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		UINT PresentFlags = (m_AllowTearing && m_WindowedMode) ? DXGI_PRESENT_ALLOW_TEARING : 0;
		ThrowIfFailed(m_d3dDeviceResources.GetSwapChain().Present(m_VSyncEnabled, PresentFlags), "Failed to present frame for D3D 12 context.");

		m_d3dDeviceResources.MoveToNextFrame();
	}

	void Direct3D12Context::OnWindowResize_Impl()
	{
		return;
		if (!m_IsMinimized) {

			if (m_WindowResizeComplete) {

				m_WindowResizeComplete = false;

				m_d3dDeviceResources.WaitForGPU();

				for (UINT i = 0; i < m_FrameBufferCount; i++) {
					m_pRenderTargetTextures[i].Reset();
					m_pSwapChainRenderTargets[i].Reset();
				}
				m_pSceneDepthStencilTexture.Reset();


				m_d3dDeviceResources.ResizeResources();

				BOOL FullScreenState;
				m_d3dDeviceResources.GetSwapChain().GetFullscreenState(&FullScreenState, nullptr);
				m_WindowedMode = !FullScreenState;
				m_d3dDeviceResources.SetFrameIndex(m_d3dDeviceResources.GetSwapChain().GetCurrentBackBufferIndex());


				UpdateSizeDependentResources();
			}
		}
		m_WindowVisible = !m_IsMinimized;
		m_WindowResizeComplete = true;
	}

	void Direct3D12Context::OnWindowFullScreen_Impl()
	{
		HWND& pHWND = m_pWindowRef->GetWindowHandleRef();

		if (m_FullScreenMode)
		{
			SetWindowLong(pHWND, GWL_STYLE, WS_OVERLAPPEDWINDOW);

			SetWindowPos(
				pHWND,
				HWND_NOTOPMOST,
				m_pWindowRef->GetWindowRect().left,
				m_pWindowRef->GetWindowRect().top,
				m_pWindowRef->GetWindowRect().right - m_pWindowRef->GetWindowRect().left,
				m_pWindowRef->GetWindowRect().bottom - m_pWindowRef->GetWindowRect().top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE
			);
			ShowWindow(pHWND, SW_NORMAL);
		}
		else
		{
			GetWindowRect(pHWND, &m_pWindowRef->GetWindowRect());

			SetWindowLong(pHWND, GWL_STYLE, WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

			RECT fullscreenWindowRect;
			try
			{
				if (&m_d3dDeviceResources.GetSwapChain())
				{
					// Get the settings of the display on which the app's window is currently displayed
					ComPtr<IDXGIOutput> pOutput;
					ThrowIfFailed(m_d3dDeviceResources.GetSwapChain().GetContainingOutput(&pOutput), "Failed to get containing output while switching to fullscreen mode in D3D 12 context.");
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
				pHWND,
				HWND_TOPMOST,
				fullscreenWindowRect.left,
				fullscreenWindowRect.top,
				fullscreenWindowRect.right,
				fullscreenWindowRect.bottom,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);


			ShowWindow(pHWND, SW_MAXIMIZE);
		}
		m_FullScreenMode = !m_FullScreenMode;
	}

	void Direct3D12Context::OnShaderReload_Impl()
	{
		m_pPostFxPass_PSO.Reset();
		m_pGeometryPass_PSO.Reset();
		m_pLightingPass_PSO.Reset();
		m_pSkyPass_PSO.Reset();
		m_pTransparency_PSO.Reset();
		m_pShadowPass_PSO.Reset();

		LoadPipelines();
	}

	void Direct3D12Context::SetVertexBuffers_Impl(uint32_t StartSlot, uint32_t NumBuffers, ieVertexBuffer* pBuffers)
	{
		IE_CORE_ASSERT(dynamic_cast<D3D12VertexBuffer*>(pBuffers) != nullptr, "A vertex buffer passed to renderer with D3D 12 active must be a \"D3D12VertexBuffer\"");
		m_pActiveCommandList->IASetVertexBuffers(StartSlot, NumBuffers, reinterpret_cast<D3D12VertexBuffer*>(pBuffers)->GetVertexBufferView());
	}

	void Direct3D12Context::SetIndexBuffer_Impl(ieIndexBuffer* pBuffer)
	{
		IE_CORE_ASSERT(dynamic_cast<D3D12IndexBuffer*>(pBuffer) != nullptr, "A index buffer passed to renderer with D3D 12 active must be a \"D3D12IndexBuffer\"");
		m_pActiveCommandList->IASetIndexBuffer(&reinterpret_cast<D3D12IndexBuffer*>(pBuffer)->GetIndexBufferView());
	}

	void Direct3D12Context::DrawIndexedInstanced_Impl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation)
	{
		m_pActiveCommandList->DrawIndexedInstanced(IndexCountPerInstance, NumInstances, StartIndexLocation, BaseVertexLoaction, StartInstanceLocation);
	}

	void Direct3D12Context::DrawText_Impl(const char* Text)
	{
		//m_SpriteBatch->Begin(m_pPostEffectsPass_CommandList.Get());
		//m_pFont->DrawString(m_SpriteBatch.get(), Text, )
		//m_SpriteBatch->End();
	}

	void Direct3D12Context::RenderSkySphere_Impl()
	{
		m_pSkySphere_Geometry->Render(m_pScenePass_CommandList);
	}

	bool Direct3D12Context::CreateSkybox_Impl()
	{
		m_pSkySphere_Geometry = new ieD3D12SphereRenderer();
		m_pSkySphere_Geometry->Init(10, 20, 20);
		return true;
	}

	void Direct3D12Context::DestroySkybox_Impl()
	{
		if (m_pSkySphere) {
			delete m_pSkySphere;
		}
	}

	uint32_t Direct3D12Context::RegisterGeometryWithRTAccelerationStucture(ComPtr<ID3D12Resource> pVertexBuffer, ComPtr<ID3D12Resource> pIndexBuffer, uint32_t NumVerticies, uint32_t NumIndices, DirectX::XMMATRIX MeshWorldMat)
	{
		return m_RTHelper.RegisterBottomLevelASGeometry(pVertexBuffer, pIndexBuffer, NumVerticies, NumIndices, MeshWorldMat);
	}

	void Direct3D12Context::CreateSwapChainRTVDescriptorHeap()
	{
		HRESULT hr;
		m_d3dDeviceResources.WaitForGPU();

		m_SwapChainRTVHeap.Create(&m_d3dDeviceResources.GetDeviceContext(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_FrameBufferCount, false);

		for (UINT i = 0; i < m_FrameBufferCount; i++) {
			hr = m_d3dDeviceResources.GetSwapChain().GetBuffer(i, IID_PPV_ARGS(&m_pSwapChainRenderTargets[i]));
			ThrowIfFailed(hr, "Failed to get buffer in swap chain during descriptor heap initialization for D3D 12 context.");
			m_d3dDeviceResources.GetDeviceContext().CreateRenderTargetView(m_pSwapChainRenderTargets[i].Get(), nullptr, m_SwapChainRTVHeap.hCPU(i));

			WCHAR name[25];
			swprintf_s(name, L"Render Target %d", i);
			m_pSwapChainRenderTargets[i]->SetName(name);
		}
	}

	void Direct3D12Context::CreateDSVs()
	{
		HRESULT hr = S_OK;

		ID3D12Device* pDevice = &m_d3dDeviceResources.GetDeviceContext();
		m_dsvHeap.Create(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 2);
		CD3DX12_HEAP_PROPERTIES DefaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

		// Scene Depth Texture
		{
			D3D12_RESOURCE_DESC SceneDepthResourceDesc = {};
			SceneDepthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			SceneDepthResourceDesc.Alignment = 0;
			SceneDepthResourceDesc.SampleDesc = { 1, 0 };
			SceneDepthResourceDesc.MipLevels = 1;
			SceneDepthResourceDesc.Format = m_DsvFormat;
			SceneDepthResourceDesc.DepthOrArraySize = 1;
			SceneDepthResourceDesc.Width = (UINT64)m_WindowWidth;
			SceneDepthResourceDesc.Height = (UINT64)m_WindowHeight;
			SceneDepthResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			SceneDepthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			D3D12_CLEAR_VALUE SceneDepthOptomizedClearValue = {};
			SceneDepthOptomizedClearValue.Format = m_DsvFormat;
			SceneDepthOptomizedClearValue.DepthStencil = { m_DepthClearValue, 0 };

			hr = pDevice->CreateCommittedResource(
				&DefaultHeapProps,
				D3D12_HEAP_FLAG_NONE,
				&SceneDepthResourceDesc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&SceneDepthOptomizedClearValue,
				IID_PPV_ARGS(&m_pSceneDepthStencilTexture));
			if (FAILED(hr))
				IE_CORE_ERROR("Failed to create comitted resource for depth stencil view");
			m_pSceneDepthStencilTexture->SetName(L"Scene Depth Stencil Buffer");

			D3D12_DEPTH_STENCIL_VIEW_DESC SceneDSVDesc = {};
			SceneDSVDesc.Texture2D.MipSlice = 0;
			SceneDSVDesc.Format = SceneDepthResourceDesc.Format;
			SceneDSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			SceneDSVDesc.Flags = D3D12_DSV_FLAG_NONE;
			pDevice->CreateDepthStencilView(m_pSceneDepthStencilTexture.Get(), &SceneDSVDesc, m_dsvHeap.hCPU(0));

			D3D12_SHADER_RESOURCE_VIEW_DESC SceneDSVSRV = {};
			SceneDSVSRV.Texture2D.MipLevels = SceneDepthResourceDesc.MipLevels;
			SceneDSVSRV.Texture2D.MostDetailedMip = 0;
			SceneDSVSRV.Format = m_SceneDepthSRVFormat;
			SceneDSVSRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			SceneDSVSRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			pDevice->CreateShaderResourceView(m_pSceneDepthStencilTexture.Get(), &SceneDSVSRV, m_cbvsrvHeap.hCPU(4));
		}

		// Shadow Depth Texture 
		{
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

			hr = pDevice->CreateCommittedResource(
				&DefaultHeapProps,
				D3D12_HEAP_FLAG_NONE,
				&ShadowDepthResourceDesc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&ShadowDepthOptomizedClearValue,
				IID_PPV_ARGS(&m_pShadowDepthTexture));
			if (FAILED(hr))
				IE_CORE_ERROR("Failed to create comitted resource for depth stencil view");
			m_pShadowDepthTexture->SetName(L"Shadow Depth Buffer");
			pDevice->CreateDepthStencilView(m_pShadowDepthTexture.Get(), &ShadowDepthDesc, m_dsvHeap.hCPU(1));

			D3D12_SHADER_RESOURCE_VIEW_DESC ShadowDSVSRV = {};
			ShadowDSVSRV.Texture2D.MipLevels = ShadowDepthResourceDesc.MipLevels;
			ShadowDSVSRV.Texture2D.MostDetailedMip = 0;
			ShadowDSVSRV.Format = DXGI_FORMAT_R32_FLOAT;
			ShadowDSVSRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			ShadowDSVSRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			pDevice->CreateShaderResourceView(m_pShadowDepthTexture.Get(), &ShadowDSVSRV, m_cbvsrvHeap.hCPU(7));
		}
	}

	void Direct3D12Context::CreateRTVs()
	{
		HRESULT hr = S_OK;

		ID3D12Device* pDevice = &m_d3dDeviceResources.GetDeviceContext();

		hr = m_rtvHeap.Create(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_NumRTVs);
		ThrowIfFailed(hr, "Failed to create render target view descriptor heap for D3D 12 context.");

		CD3DX12_HEAP_PROPERTIES DefaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

		D3D12_RESOURCE_DESC ResourceDesc = {};
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Alignment = 0;
		ResourceDesc.SampleDesc = { 1, 0 };
		ResourceDesc.MipLevels = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.Width = (UINT)m_WindowWidth;
		ResourceDesc.Height = (UINT)m_WindowHeight;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE ClearVal;
		for (uint8_t i = 0; i < 4; i++)
			ClearVal.Color[i] = m_ScreenClearColor[i];

		for (uint8_t i = 0; i < m_NumRTVs; i++) {
			ResourceDesc.Format = m_RtvFormat[i];
			ClearVal.Format = m_RtvFormat[i];
			hr = pDevice->CreateCommittedResource(&DefaultHeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &ClearVal, IID_PPV_ARGS(&m_pRenderTargetTextures[i]));
			ThrowIfFailed(hr, "Failed to create committed resource for RTV at index: " + std::to_string(i));
		}
		m_pRenderTargetTextures[0]->SetName(L"Render Target Texture Diffuse");
		m_pRenderTargetTextures[1]->SetName(L"Render Target Texture Normal");
		m_pRenderTargetTextures[2]->SetName(L"Render Target Texture (R)Roughness/(G)Metallic/(B)AO");
		m_pRenderTargetTextures[3]->SetName(L"Render Target Texture Position");
		m_pRenderTargetTextures[4]->SetName(L"Render Target Texture Light Pass Result");
		m_pRenderTargetTextures[5]->SetName(L"Render Target Texture Bloom");


		D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = {};
		RTVDesc.Texture2D.MipSlice = 0;
		RTVDesc.Texture2D.PlaneSlice = 0;
		RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		for (uint8_t i = 0; i < m_NumRTVs; i++) {
			RTVDesc.Format = m_RtvFormat[i];
			pDevice->CreateRenderTargetView(m_pRenderTargetTextures[i].Get(), &RTVDesc, m_rtvHeap.hCPU(i));
		}

		//Create SRVs for Render Targets
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Texture2D.MipLevels = ResourceDesc.MipLevels;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Format = ResourceDesc.Format;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;


		for (uint8_t i = 0; i < m_NumRTVs - 2; i++) {
			SRVDesc.Format = m_RtvFormat[i];
			pDevice->CreateShaderResourceView(m_pRenderTargetTextures[i].Get(), &SRVDesc, m_cbvsrvHeap.hCPU(i));
		}
		m_pRenderTargetTextures[0]->SetName(L"Render Target SRV Albedo");
		m_pRenderTargetTextures[1]->SetName(L"Render Target SRV Normal");
		m_pRenderTargetTextures[2]->SetName(L"Render Target SRV (R)Roughness/(G)Metallic/(B)AO");
		m_pRenderTargetTextures[3]->SetName(L"Render Target SRV Position");

		// Light Pass Buffer
		SRVDesc.Format = m_RtvFormat[4];
		pDevice->CreateShaderResourceView(m_pRenderTargetTextures[4].Get(), &SRVDesc, m_cbvsrvHeap.hCPU(5));
		m_pRenderTargetTextures[4]->SetName(L"Render Target SRV Light Pass Result");

		// Bloom Buffer/s
		SRVDesc.Format = m_RtvFormat[5];
		pDevice->CreateShaderResourceView(m_pRenderTargetTextures[5].Get(), &SRVDesc, m_cbvsrvHeap.hCPU(8));
		m_pRenderTargetTextures[5]->SetName(L"Render Target SRV Bloom Buffer");

		static bool Created = false;
		if (Created) return;
		Created = true;

		// UAV Down-Sampled Buffer
		ResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		ResourceDesc.Width = m_WindowWidth / 2;
		ResourceDesc.Height = m_WindowHeight / 2;
		hr = pDevice->CreateCommittedResource(&DefaultHeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&m_pBloomBlurResult_UAV));
		ThrowIfFailed(hr, "Failed to create committed resource for bloom down sampled UAV.");
		D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
		UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		pDevice->CreateUnorderedAccessView(m_pBloomBlurResult_UAV.Get(), nullptr, &UAVDesc, m_cbvsrvHeap.hCPU(9));
		m_pBloomBlurResult_UAV->SetName(L"UAV: Bloom Pass Down Sampled Buffer");

		// UAV Down-Sampled Intermediate Buffer
		hr = pDevice->CreateCommittedResource(&DefaultHeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&m_pBloomBlurIntermediateBuffer_UAV));
		ThrowIfFailed(hr, "Failed to create committed resource for bloom down sampled intermdiate UAV.");
		pDevice->CreateUnorderedAccessView(m_pBloomBlurIntermediateBuffer_UAV.Get(), nullptr, &UAVDesc, m_cbvsrvHeap.hCPU(11));
		m_pBloomBlurIntermediateBuffer_UAV->SetName(L"UAV: Bloom Pass Down Sampled INTERMEDIENT Buffer");
	}

	void Direct3D12Context::CreateSRVs()
	{
		HRESULT hr;

		ID3D12Device* pDevice = &m_d3dDeviceResources.GetDeviceContext();
		CD3DX12_HEAP_PROPERTIES DefaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

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

		// Ray Trace Pass Result SRV
		hr = pDevice->CreateCommittedResource(&DefaultHeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, NULL, IID_PPV_ARGS(&m_RayTraceOutput_SRV));
		ThrowIfFailed(hr, "Failed to create ray trace output srv");
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDesc.Texture2D.MipLevels = 1U;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		pDevice->CreateShaderResourceView(m_RayTraceOutput_SRV.Get(), &SRVDesc, m_cbvsrvHeap.hCPU(6));


		// SRV Down-Sampled Buffer
		ResourceDesc.Width = (UINT)m_WindowWidth / 2;
		ResourceDesc.Height = (UINT)m_WindowHeight / 2;
		hr = pDevice->CreateCommittedResource(&DefaultHeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(&m_pBloomBlurResult_SRV));
		ThrowIfFailed(hr, "Failed to create committed resource for down sampled UAV.");
		pDevice->CreateShaderResourceView(m_pBloomBlurResult_SRV.Get(), &SRVDesc, m_cbvsrvHeap.hCPU(10));
		m_pBloomBlurResult_SRV->SetName(L"SRV: Bloom Pass Down Sampled Buffer");

		// Bloom Blur Intermediate Buffer
		ThrowIfFailed(hr, "Failed to create committed resource for down sampled UAV.");
		hr = pDevice->CreateCommittedResource(&DefaultHeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(&m_pBloomBlurIntermediateBuffer_SRV));
		pDevice->CreateShaderResourceView(m_pBloomBlurIntermediateBuffer_SRV.Get(), &SRVDesc, m_cbvsrvHeap.hCPU(12));
		m_pBloomBlurIntermediateBuffer_SRV->SetName(L"SRV: Bloom Pass Down Sampled INTERMEDIENT Buffer");
	}

	void Direct3D12Context::CreateDeferredShadingRS()
	{
		CD3DX12_DESCRIPTOR_RANGE DescriptorRanges[14] = {};
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
		DescriptorRanges[13].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1); // Post-FX input Bloom pass result

		CD3DX12_ROOT_PARAMETER RootParameters[19] = {};
		RootParameters[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);	  // Per-Object constant buffer
		RootParameters[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);		  // Per-Frame constant buffer
		RootParameters[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);		  // Light constant buffer
		RootParameters[3].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_PIXEL);	  // PostFx constant buffer
		RootParameters[4].InitAsConstantBufferView(4, 0, D3D12_SHADER_VISIBILITY_ALL);		  // Material Additives constant buffer
		RootParameters[5].InitAsDescriptorTable(1, &DescriptorRanges[0], D3D12_SHADER_VISIBILITY_PIXEL);  // G-Buffer inputs

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
		RootParameters[18].InitAsDescriptorTable(1, &DescriptorRanges[13], D3D12_SHADER_VISIBILITY_PIXEL); // Bloom Image




		
		CD3DX12_STATIC_SAMPLER_DESC StaticSamplers[3] = {};
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
		constexpr FLOAT MinLOD = 0.0f, MaxLOD = 9.0f;
		StaticSamplers[1].Init(
			1,
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,//D3D12_FILTER_ANISOTROPIC
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
		// CubeMap Sampler
		StaticSamplers[2].Init(
			2,
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			0,
			16,
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
			MinLOD,
			MaxLOD,
			D3D12_SHADER_VISIBILITY_PIXEL,
			0U
		);

		CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc;
		RootSignatureDesc.Init(_countof(RootParameters), RootParameters, _countof(StaticSamplers), StaticSamplers, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


		ComPtr<ID3DBlob> RootSignatureBlob;
		ComPtr<ID3DBlob> ErrorBlob;

		HRESULT hr = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSignatureBlob.GetAddressOf(), ErrorBlob.GetAddressOf());
		ThrowIfFailed(hr, "Failed to serialize root signature for D3D 12 context.");

		hr = m_d3dDeviceResources.GetDeviceContext().CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pDeferredShadingPass_RS));
		ThrowIfFailed(hr, "Failed to create root signature for D3D 12 context.");
	}

	void Direct3D12Context::CreateForwardShadingRS()
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

		hr = m_d3dDeviceResources.GetDeviceContext().CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pForwardShadingPass_RS));
		ThrowIfFailed(hr, "Failed to create root signature for D3D 12 context.");
	}

	void Direct3D12Context::CreateBloomPassRS()
	{
		CD3DX12_DESCRIPTOR_RANGE DescriptorRanges[2] = {};
		DescriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // Bloom SRV Source
		DescriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0); // Bloom UAV Destination

		CD3DX12_ROOT_PARAMETER RootParameters[3] = {};
		RootParameters[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); // Ping buffer
		RootParameters[1].InitAsDescriptorTable(1, &DescriptorRanges[0], D3D12_SHADER_VISIBILITY_ALL); // Source Texture
		RootParameters[2].InitAsDescriptorTable(1, &DescriptorRanges[1], D3D12_SHADER_VISIBILITY_ALL); // Destination Texture

		CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc;
		RootSignatureDesc.Init(_countof(RootParameters), RootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE);

		ComPtr<ID3DBlob> RootSignatureBlob;
		ComPtr<ID3DBlob> ErrorBlob;

		HRESULT hr = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSignatureBlob.GetAddressOf(), ErrorBlob.GetAddressOf());
		ThrowIfFailed(hr, "Failed to serialize root signature for D3D 12 context.");

		hr = m_d3dDeviceResources.GetDeviceContext().CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pBloomPass_RS));
		ThrowIfFailed(hr, "Failed to create root signature for D3D 12 context.");
	}

	void Direct3D12Context::CreateDebugScreenQuadRS()
	{
		CD3DX12_DESCRIPTOR_RANGE DescriptorRanges[1] = {};
		DescriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // Debug Texture

		CD3DX12_ROOT_PARAMETER RootParameters[3] = {};
		RootParameters[0].InitAsDescriptorTable(1, &DescriptorRanges[0], D3D12_SHADER_VISIBILITY_PIXEL); // Debug Texture
		RootParameters[1].InitAsConstantBufferView(0); // Light Buffer
		RootParameters[2].InitAsConstantBufferView(1); // PerFrame Buffer

		CD3DX12_STATIC_SAMPLER_DESC StaticSamplers[1] = {};
		StaticSamplers[0].Init(
			0,
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			0,
			4,
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
			0,
			1,
			D3D12_SHADER_VISIBILITY_PIXEL,
			0U
		);

		CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc;
		RootSignatureDesc.Init(_countof(RootParameters), RootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		RootSignatureDesc.NumStaticSamplers = _countof(StaticSamplers);
		RootSignatureDesc.pStaticSamplers = StaticSamplers;

		ComPtr<ID3DBlob> RootSignatureBlob;
		ComPtr<ID3DBlob> ErrorBlob;

		HRESULT hr = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSignatureBlob.GetAddressOf(), ErrorBlob.GetAddressOf());
		ThrowIfFailed(hr, "Failed to serialize root signature for D3D 12 context.");

		hr = m_d3dDeviceResources.GetDeviceContext().CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pDebugScreenQuad_RS));
		ThrowIfFailed(hr, "Failed to create root signature for D3D 12 context.");
	}

	void Direct3D12Context::LoadPipelines()
	{
		HRESULT hr = S_OK;

		// Create Shadow Pass
		{
			ComPtr<ID3DBlob> pVertexShader;
			ComPtr<ID3DBlob> pPixelShader;

			const std::wstring_view ExeDirectory = FileSystem::GetExecutbleDirectoryW();
			std::wstring VertShaderFolder(ExeDirectory);
			VertShaderFolder += L"../Renderer/Shadow_Pass.vertex.cso";
			std::wstring PixelShaderFolder(ExeDirectory);
			PixelShaderFolder += L"../Renderer/Shadow_Pass.pixel.cso";

			hr = D3DReadFileToBlob(VertShaderFolder.c_str(), &pVertexShader);
			ThrowIfFailed(hr, "Failed to read Vertex Shader for D3D 12 context.");
			hr = D3DReadFileToBlob(PixelShaderFolder.c_str(), &pPixelShader);
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
			PsoDesc.pRootSignature = m_pDeferredShadingPass_RS.Get();
			PsoDesc.DepthStencilState = ShadowDepthStencilDesc;
			PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			PsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			PsoDesc.SampleMask = UINT_MAX;
			PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			PsoDesc.DSVFormat = m_ShadowMapFormat;
			PsoDesc.SampleDesc.Count = 1;
			PsoDesc.NumRenderTargets = 0;
			PsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;

			hr = m_d3dDeviceResources.GetDeviceContext().CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&m_pShadowPass_PSO));
			ThrowIfFailed(hr, "Failed to create graphics pipeline state for shadow pass in D3D 12 context.");
			m_pShadowPass_PSO->SetName(L"PSO Shadow Pass");
		}
		
		// Create Geometry Pass
		/*{
			ComPtr<ID3DBlob> pVertexShader;
			ComPtr<ID3DBlob> pPixelShader;

			const std::wstring_view ExeDirectory = FileSystem::GetExecutbleDirectoryW();
			std::wstring VertexShaderFolder(ExeDirectory);
			VertexShaderFolder += L"../Renderer/Geometry_Pass.vertex.cso";
			std::wstring PixelShaderFolder(ExeDirectory);
			PixelShaderFolder += L"../Renderer/Geometry_Pass.pixel.cso";

			hr = D3DReadFileToBlob(VertexShaderFolder.c_str(), &pVertexShader);
			ThrowIfFailed(hr, "Failed to read Vertex Shader for D3D 12 context.");
			hr = D3DReadFileToBlob(PixelShaderFolder.c_str(), &pPixelShader);
			ThrowIfFailed(hr, "Failed to read Pixel Shader for D3D 12 context.");


			D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
			vertexShaderBytecode.BytecodeLength = pVertexShader->GetBufferSize();
			vertexShaderBytecode.pShaderBytecode = pVertexShader->GetBufferPointer();

			D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
			pixelShaderBytecode.BytecodeLength = pPixelShader->GetBufferSize();
			pixelShaderBytecode.pShaderBytecode = pPixelShader->GetBufferPointer();

			D3D12_INPUT_ELEMENT_DESC inputLayout[5] =
			{
				{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0  },
				{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0  },
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
			PsoDesc.pRootSignature = m_pDeferredShadingPass_RS.Get();
			PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			PsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			PsoDesc.SampleMask = UINT_MAX;
			PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			PsoDesc.NumRenderTargets = m_NumGBuffers;
			for (UINT i = 0; i < m_NumGBuffers; ++i)
				PsoDesc.RTVFormats[i] = m_RtvFormat[i];
			PsoDesc.DSVFormat = m_DsvFormat;
			PsoDesc.SampleDesc = { 1, 0 };

			hr = m_d3dDeviceResources.GetDeviceContext().CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&m_pGeometryPass_PSO));
			ThrowIfFailed(hr, "Failed to create graphics pipeline state for geometry pass.");
			m_pGeometryPass_PSO->SetName(L"PSO Geometry Pass");
		}*/

		// Create Light Pass
		{
			ComPtr<ID3DBlob> pVertexShader;
			ComPtr<ID3DBlob> pPixelShader;

			const std::wstring_view ExeDirectory = FileSystem::GetExecutbleDirectoryW();
			std::wstring VertexShaderFolder(ExeDirectory);
			VertexShaderFolder += L"../Renderer/Light_Pass.vertex.cso";
			std::wstring PixelShaderFolder(ExeDirectory);
			PixelShaderFolder += L"../Renderer/Light_Pass.pixel.cso";

			hr = D3DReadFileToBlob(VertexShaderFolder.c_str(), &pVertexShader);
			ThrowIfFailed(hr, "Failed to compile Vertex Shader for D3D 12 context.");
			hr = D3DReadFileToBlob(PixelShaderFolder.c_str(), &pPixelShader);
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
			descPipelineState.pRootSignature = m_pDeferredShadingPass_RS.Get();
			descPipelineState.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			descPipelineState.DepthStencilState.DepthEnable = false;
			descPipelineState.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			descPipelineState.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			descPipelineState.RasterizerState.DepthClipEnable = false;
			descPipelineState.SampleMask = UINT_MAX;
			descPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			descPipelineState.NumRenderTargets = 2;
			descPipelineState.RTVFormats[0] = m_RtvFormat[4]; // Light Pass
			descPipelineState.RTVFormats[1] = m_RtvFormat[5]; // Bloom Buffer
			descPipelineState.SampleDesc.Count = 1;

			hr = m_d3dDeviceResources.GetDeviceContext().CreateGraphicsPipelineState(&descPipelineState, IID_PPV_ARGS(&m_pLightingPass_PSO));
			ThrowIfFailed(hr, "Failed to create graphics pipeline state for lighting pass.");
			m_pLightingPass_PSO->SetName(L"PSO Light Pass");
		}
		
		// Create Sky Pass
		{
			ComPtr<ID3DBlob> pVertexShader;
			ComPtr<ID3DBlob> pPixelShader;

			const std::wstring_view ExeDirectory = FileSystem::GetExecutbleDirectoryW();
			std::wstring VertexShaderFolder(ExeDirectory);
			VertexShaderFolder += L"../Renderer/Skybox.vertex.cso";
			std::wstring PixelShaderFolder(ExeDirectory);
			PixelShaderFolder += L"../Renderer/Skybox.pixel.cso";

			hr = D3DReadFileToBlob(VertexShaderFolder.c_str(), &pVertexShader);
			ThrowIfFailed(hr, "Failed to compile Vertex Shader for D3D 12 context");
			hr = D3DReadFileToBlob(PixelShaderFolder.c_str(), &pPixelShader);
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
			pipelineDesc.pRootSignature = m_pDeferredShadingPass_RS.Get();
			pipelineDesc.DepthStencilState = depthStencilStateDesc;
			pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			pipelineDesc.RasterizerState = rasterizerStateDesc;
			pipelineDesc.SampleMask = UINT_MAX;
			pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			pipelineDesc.NumRenderTargets = 1;
			pipelineDesc.RTVFormats[0] = m_RtvFormat[4];
			pipelineDesc.SampleDesc.Count = 1;
			pipelineDesc.DSVFormat = m_DsvFormat;


			hr = m_d3dDeviceResources.GetDeviceContext().CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&m_pSkyPass_PSO));
			ThrowIfFailed(hr, "Failed to create skybox pipeline state object for .");
			m_pSkyPass_PSO->SetName(L"PSO Sky Pass");
		}

		// Create Transparency Pass
		{
			ComPtr<ID3DBlob> pVertexShader;
			ComPtr<ID3DBlob> pPixelShader;

			const std::wstring_view ExeDirectory = FileSystem::GetExecutbleDirectoryW();
			std::wstring VertexShaderFolder(ExeDirectory);
			VertexShaderFolder += L"../Renderer/Transparency_Pass.vertex.cso";
			std::wstring PixelShaderFolder(ExeDirectory);
			PixelShaderFolder += L"../Renderer/Transparency_Pass.pixel.cso";

			hr = D3DReadFileToBlob(VertexShaderFolder.c_str(), &pVertexShader);
			ThrowIfFailed(hr, "Failed to read Vertex Shader for D3D 12 context.");
			hr = D3DReadFileToBlob(PixelShaderFolder.c_str(), &pPixelShader);
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
			PsoDesc.pRootSignature = m_pForwardShadingPass_RS.Get();
			PsoDesc.DepthStencilState = DepthStencilStateDesc;
			PsoDesc.BlendState = BlendDesc;
			PsoDesc.RasterizerState = RasterizerDesc;
			PsoDesc.SampleMask = UINT_MAX;
			PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			PsoDesc.NumRenderTargets = 1U;
			PsoDesc.RTVFormats[0] = m_RtvFormat[4];
			//PsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			PsoDesc.DSVFormat = m_DsvFormat;
			PsoDesc.SampleDesc.Count = 1;

			hr = m_d3dDeviceResources.GetDeviceContext().CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&m_pTransparency_PSO));
			ThrowIfFailed(hr, "Failed to create graphics pipeline state for transparency pass.");
			m_pTransparency_PSO->SetName(L"PSO Transparency Pass");
		}

		// Create Post Process Pass
		{
			ComPtr<ID3DBlob> pVertexShader;
			ComPtr<ID3DBlob> pPixelShader;

			const std::wstring_view ExeDirectory = FileSystem::GetExecutbleDirectoryW();
			std::wstring VertexShaderFolder(ExeDirectory);
			VertexShaderFolder += L"../Renderer/Screen_Aligned_Quad.vertex.cso";
			std::wstring PixelShaderFolder(ExeDirectory);
			PixelShaderFolder += L"../Renderer/PostProcess_Composite.pixel.cso";

			hr = D3DReadFileToBlob(VertexShaderFolder.c_str(), &pVertexShader);
			ThrowIfFailed(hr, "Failed to compile Vertex Shader for D3D 12 context.");
			hr = D3DReadFileToBlob(PixelShaderFolder.c_str(), &pPixelShader);
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
			descPipelineState.pRootSignature = m_pDeferredShadingPass_RS.Get();
			descPipelineState.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			descPipelineState.DepthStencilState.DepthEnable = false;
			descPipelineState.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			descPipelineState.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			descPipelineState.RasterizerState.DepthClipEnable = false;
			descPipelineState.SampleMask = UINT_MAX;
			descPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			descPipelineState.NumRenderTargets = 1;
			descPipelineState.RTVFormats[0] = m_d3dDeviceResources.GetSwapChainBackBufferFormat();
			descPipelineState.SampleDesc.Count = 1;

			hr = m_d3dDeviceResources.GetDeviceContext().CreateGraphicsPipelineState(&descPipelineState, IID_PPV_ARGS(&m_pPostFxPass_PSO));
			ThrowIfFailed(hr, "Failed to create graphics pipeline state for Post-Fx pass in D3D 12 context.");
			m_pPostFxPass_PSO->SetName(L"PSO PostFx Pass");
		}
		
		// Create Debug Screen Quad Pass
		{
			ComPtr<ID3DBlob> pVertexShader;
			ComPtr<ID3DBlob> pPixelShader;

			const std::wstring_view ExeDirectory = FileSystem::GetExecutbleDirectoryW();
			std::wstring VertexShaderFolder(ExeDirectory);
			VertexShaderFolder += L"../Renderer/Screen_Aligned_Quad.vertex.cso";
			std::wstring PixelShaderFolder(ExeDirectory);
			PixelShaderFolder += L"../Renderer/Debug_Screen_Quad.pixel.cso";

			hr = D3DReadFileToBlob(VertexShaderFolder.c_str(), &pVertexShader);
			ThrowIfFailed(hr, "Failed to compile Vertex Shader for D3D 12 context.");
			hr = D3DReadFileToBlob(PixelShaderFolder.c_str(), &pPixelShader);
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
			descPipelineState.pRootSignature = m_pDebugScreenQuad_RS.Get();
			descPipelineState.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			descPipelineState.DepthStencilState.DepthEnable = false;
			descPipelineState.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			descPipelineState.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			descPipelineState.RasterizerState.DepthClipEnable = false;
			descPipelineState.SampleMask = UINT_MAX;
			descPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			descPipelineState.NumRenderTargets = 1;
			descPipelineState.RTVFormats[0] = m_d3dDeviceResources.GetSwapChainBackBufferFormat();
			descPipelineState.SampleDesc.Count = 1;

			hr = m_d3dDeviceResources.GetDeviceContext().CreateGraphicsPipelineState(&descPipelineState, IID_PPV_ARGS(&m_pDebugScreenQuad_PSO));
			ThrowIfFailed(hr, "Failed to create graphics pipeline state for Debug Screen Quad pass in D3D 12 context.");
			m_pDebugScreenQuad_PSO->SetName(L"PSO Debug Screen Quad Pass");
		}

		// Create Down Sample Pass
		{
			ComPtr<ID3DBlob> pComputeShader;

			const std::wstring_view ExeDirectory = FileSystem::GetExecutbleDirectoryW();
			std::wstring VertexShaderFolder(ExeDirectory);
			VertexShaderFolder += L"../Renderer/Threshold_Down_Sample.compute.cso";

			hr = D3DReadFileToBlob(VertexShaderFolder.c_str(), &pComputeShader);
			ThrowIfFailed(hr, "Failed to read compute shader for D3D 12 context");

			D3D12_SHADER_BYTECODE ComputeShaderBytecode = {};
			ComputeShaderBytecode.BytecodeLength = pComputeShader->GetBufferSize();
			ComputeShaderBytecode.pShaderBytecode = pComputeShader->GetBufferPointer();

			D3D12_COMPUTE_PIPELINE_STATE_DESC PipelineDesc = {};
			PipelineDesc.CS = ComputeShaderBytecode;
			PipelineDesc.pRootSignature = m_pBloomPass_RS.Get();

			hr = m_d3dDeviceResources.GetDeviceContext().CreateComputePipelineState(&PipelineDesc, IID_PPV_ARGS(&m_pThresholdDownSample_PSO));
			ThrowIfFailed(hr, "Failed to create texture downsampling pipeline for bloom pass.");
			m_pThresholdDownSample_PSO->SetName(L"Bloom Pass PSO");
		}

		// Create Gausian Blur Pass
		{
			ComPtr<ID3DBlob> pComputeShader;

			const std::wstring_view ExeDirectory = FileSystem::GetExecutbleDirectoryW();
			std::wstring ComputeShaderFolder(ExeDirectory);
			ComputeShaderFolder += L"../Renderer/Gaussian_Blur.compute.cso";

			hr = D3DReadFileToBlob(ComputeShaderFolder.c_str(), &pComputeShader);
			ThrowIfFailed(hr, "Failed to read compute shader for D3D 12 context");

			D3D12_SHADER_BYTECODE ComputeShaderBytecode = {};
			ComputeShaderBytecode.BytecodeLength = pComputeShader->GetBufferSize();
			ComputeShaderBytecode.pShaderBytecode = pComputeShader->GetBufferPointer();

			D3D12_COMPUTE_PIPELINE_STATE_DESC PipelineDesc = {};
			PipelineDesc.CS = ComputeShaderBytecode;
			PipelineDesc.pRootSignature = m_pBloomPass_RS.Get();

			hr = m_d3dDeviceResources.GetDeviceContext().CreateComputePipelineState(&PipelineDesc, IID_PPV_ARGS(&m_pGaussianBlur_PSO));
			ThrowIfFailed(hr, "Failed to create gaussian blur pipeline for bloom pass.");
			m_pGaussianBlur_PSO->SetName(L"Bloom Pass PSO");
		}

	}

	void Direct3D12Context::CreateCommandAllocators()
	{
		HRESULT hr = S_OK;

		// Scene Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_d3dDeviceResources.GetDeviceContext().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pScenePass_CommandAllocators[i]));
				ThrowIfFailed(hr, "Failed to Scene Pass Create Command Allocator for D3D 12 context");
				m_pScenePass_CommandAllocators[i]->SetName(L"Scene Pass Command Allocator");
			}

			hr = m_d3dDeviceResources.GetDeviceContext().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pScenePass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pScenePass_CommandList));
			ThrowIfFailed(hr, "Failed to Scene Pass Create Command List for D3D 12 context");
			m_pScenePass_CommandList->SetName(L"Scene Pass Command List");

			// Dont close the Scene Pass command list yet. 
			// We'll use it for application initialization.
		}

		// Shadow Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_d3dDeviceResources.GetDeviceContext().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pShadowPass_CommandAllocators[i]));
				ThrowIfFailed(hr, "Failed to Create Command Allocator for D3D 12 context");
				m_pShadowPass_CommandAllocators[i]->SetName(L"Shadow Pass Command Allocator");
			}

			hr = m_d3dDeviceResources.GetDeviceContext().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pShadowPass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pShadowPass_CommandList));
			ThrowIfFailed(hr, "Failed to Create Shadow Pass Command List for D3D 12 context");
			m_pShadowPass_CommandList->SetName(L"Shadow Pass Command List");

			m_pShadowPass_CommandList->Close();
		}

		// Transprency Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_d3dDeviceResources.GetDeviceContext().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pTransparencyPass_CommandAllocators[i]));
				ThrowIfFailed(hr, "Failed to Create Command Allocator for D3D 12 context");
				m_pTransparencyPass_CommandAllocators[i]->SetName(L"Transparency Pass Command Allocator");
			}

			hr = m_d3dDeviceResources.GetDeviceContext().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pTransparencyPass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pTransparencyPass_CommandList));
			ThrowIfFailed(hr, "Failed to Create Transparency Pass Command List for D3D 12 context");
			m_pTransparencyPass_CommandList->SetName(L"Transparency Pass Command List");

			m_pTransparencyPass_CommandList->Close();
		}

		// Post-Process Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_d3dDeviceResources.GetDeviceContext().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pPostEffectsPass_CommandAllocators[i]));
				ThrowIfFailed(hr, "Failed to Create Command Allocator for D3D 12 context");
				m_pPostEffectsPass_CommandAllocators[i]->SetName(L"Post-Process Pass Command Allocator");
			}

			hr = m_d3dDeviceResources.GetDeviceContext().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pPostEffectsPass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pPostEffectsPass_CommandList));
			ThrowIfFailed(hr, "Failed to Create Post-Process Pass Command List for D3D 12 context");
			m_pPostEffectsPass_CommandList->SetName(L"Post-Process Pass Command List");

			m_pPostEffectsPass_CommandList->Close();
		}

		// Gaussian Blur Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_d3dDeviceResources.GetDeviceContext().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&m_pDownSample_CommandAllocators[i]));
				ThrowIfFailed(hr, "Failed to Create Command Allocator for D3D 12 context");
				m_pDownSample_CommandAllocators[i]->SetName(L"Gaussian Blur Command Allocator");
			}

			hr = m_d3dDeviceResources.GetDeviceContext().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, m_pDownSample_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pDownSample_CommandList));
			ThrowIfFailed(hr, "Failed to Create Gaussian Blur Command List for D3D 12 context");
			m_pDownSample_CommandList->SetName(L"Gaussian Blur Command List");

			m_pDownSample_CommandList->Close();
		}

		// Ray Trace Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_d3dDeviceResources.GetDeviceContext().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pRayTracePass_CommandAllocators[i]));
				ThrowIfFailed(hr, "Failed to Create Command Allocator for D3D 12 context");
				m_pRayTracePass_CommandAllocators[i]->SetName(L"Post-Process Pass Command Allocator");
			}

			hr = m_d3dDeviceResources.GetDeviceContext().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pRayTracePass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pRayTracePass_CommandList));
			ThrowIfFailed(hr, "Failed to Create Post-Process Pass Command List for D3D 12 context");
			m_pRayTracePass_CommandList->SetName(L"Post-Process Pass Command List");

			// Dont close the Ray Trace Pass command list yet. 
			// We'll use it for RT pipeline initialization.
		}

		ID3D12CommandList* ppCommandLists[] = {
			m_pShadowPass_CommandList.Get(),
			m_pTransparencyPass_CommandList.Get(),
			m_pPostEffectsPass_CommandList.Get()
		};
		m_d3dDeviceResources.GetGraphicsCommandQueue().ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		m_d3dDeviceResources.IncrementAndSignalFence();
		m_d3dDeviceResources.WaitForGPU();
	}

	void Direct3D12Context::CreateViewport()
	{
		m_ShadowPass_ViewPort.TopLeftX = 0;
		m_ShadowPass_ViewPort.TopLeftY = 0;
		m_ShadowPass_ViewPort.Width = static_cast<FLOAT>(m_ShadowMapWidth);
		m_ShadowPass_ViewPort.Height = static_cast<FLOAT>(m_ShadowMapHeight);
		m_ShadowPass_ViewPort.MinDepth = 0.0f;
		m_ShadowPass_ViewPort.MaxDepth = 1.0f;
	}

	void Direct3D12Context::CreateScissorRect()
	{
		m_ShadowPass_ScissorRect.left = 0;
		m_ShadowPass_ScissorRect.top = 0;
		m_ShadowPass_ScissorRect.right = m_ShadowMapWidth;
		m_ShadowPass_ScissorRect.bottom = m_ShadowMapHeight;
	}

	void Direct3D12Context::CreateScreenQuad()
	{
		uint32_t QuadIndices[] =
		{
			0, 1, 3,
			0, 3, 2
		};

		ScreenSpaceVertex FullScreenQuadVerts[] =
		{
			{ { -1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } }, // Top Left
			{ {  1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } }, // Top Right
			{ { -1.0f,-1.0f, 0.0f }, { 0.0f, 1.0f } }, // Bottom Left
			{ {  1.0f,-1.0f, 0.0f }, { 1.0f, 1.0f } }, // Bottom Right
		};
		m_ScreenQuad.Init(FullScreenQuadVerts, sizeof(FullScreenQuadVerts), QuadIndices, sizeof(QuadIndices));


		ScreenSpaceVertex DebugScreenQuadVerts[] =
		{
			{ { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } }, // Top Left
			{ {  0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } }, // Top Right
			{ { -1.0f,-1.0f, 0.0f }, { 0.0f, 1.0f } }, // Bottom Left
			{ {  0.0f,-1.0f, 0.0f }, { 1.0f, 1.0f } }, // Bottom Right
		};
		m_DebugScreenQuad.Init(DebugScreenQuadVerts, sizeof(DebugScreenQuadVerts), QuadIndices, sizeof(QuadIndices));
	}

	void Direct3D12Context::CloseCommandListAndSignalCommandQueue()
	{
		// Generate the acceleration structures for the ray tracer
		if (m_GraphicsSettings.RayTraceEnabled) m_RTHelper.GenerateAccelerationStructure();

		m_pScenePass_CommandList->Close();
		m_pRayTracePass_CommandList->Close();

		ID3D12CommandList* ppCommandLists[] = {
			m_pScenePass_CommandList.Get(),
			m_pRayTracePass_CommandList.Get()
		};
		m_d3dDeviceResources.GetGraphicsCommandQueue().ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		m_d3dDeviceResources.IncrementAndSignalFence();
		if (m_GraphicsSettings.RayTraceEnabled) m_RTHelper.OnPostInit();

		m_d3dDeviceResources.WaitForGPU();
	}

	void Direct3D12Context::UpdateSizeDependentResources()
	{
		// Re-Create Render Target View
		{
			for (UINT i = 0; i < m_FrameBufferCount; i++)
			{
				m_d3dDeviceResources.GetSwapChain().GetBuffer(i, IID_PPV_ARGS(&m_pSwapChainRenderTargets[i]));
				m_d3dDeviceResources.GetDeviceContext().CreateRenderTargetView(m_pSwapChainRenderTargets[i].Get(), nullptr, m_rtvHeap.hCPU(i));
			}
		}

		// Re-Create Depth Stencil View
		{
			CreateDSVs();
			CreateRTVs();
			//m_RTHelper.ReCreateOutputBuffer();
			//CreateSRVs();
			CreateSwapChainRTVDescriptorHeap();
		}

		// Recreate Camera Projection Matrix
		{
			if (!m_pWorldCameraRef->GetIsOrthographic()) {
				m_pWorldCameraRef->SetPerspectiveProjectionValues(m_pWorldCameraRef->GetFOV(), static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight), m_pWorldCameraRef->GetNearZ(), m_pWorldCameraRef->GetFarZ());
			}
		}

	}

	void Direct3D12Context::ResourceBarrier(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter)
	{
		pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pResource, StateBefore, StateAfter));
	}

}