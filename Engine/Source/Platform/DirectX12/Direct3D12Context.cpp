#include <Engine_pch.h>

#include "Direct3D12Context.h"

#include "Runtime/Core/Application.h"
#include "Platform/Win32/Win32Window.h"
#include "Runtime/GameFramework/Archetypes/APlayerCharacter.h"
#include "Runtime/Systems/Managers/GeometryManager.h"
#include "Platform/DirectX12/RayTracing/RayTraceHelpers.h"

#include "Runtime/Graphics/APostFx.h"
#include "Runtime/Graphics/ASkyLight.h"
#include "Runtime/Graphics/ASkySphere.h"
#include "Runtime/Graphics/Lighting/ASpotLight.h"
#include "Runtime/Graphics/Lighting/APointLight.h"
#include "Runtime/Graphics/Lighting/ADirectionalLight.h"

#include "Platform/DirectX12/Geometry/D3D12VertexBuffer.h"
#include "Platform/DirectX12/Geometry/D3D12IndexBuffer.h"
#include "Platform/DirectX12/Geometry/D3D12SphereRenderer.h"

#include "Runtime/UI/UILib.h"


#define SHADOWMAPPING_ENABLED 0
#define TRANSPARENCYPASS_ENABLED 0
#define BLOOM_ENABLED 0


namespace Insight {


	Direct3D12Context::Direct3D12Context()
	{
	}

	Direct3D12Context::~Direct3D12Context()
	{
		InternalCleanup();
	}

	void Direct3D12Context::InternalCleanup()
	{
		// Wait until GPU is done consuming all data
		// before closing all handles and releasing resources
		m_DeviceResources.WaitForGPU();

		if (!m_AllowTearing)
			m_DeviceResources.GetSwapChain().SetFullscreenState(false, NULL);

		m_DeviceResources.CleanUp();
	}

	bool Direct3D12Context::Init_Impl()
	{
		IE_LOG(Log, TEXT("Renderer: D3D 12"));

		try
		{
			m_DeviceResources.Init(this);

			D3D12_FEATURE_DATA_SHADER_MODEL sm6_0{ D3D_SHADER_MODEL_6_0 };
			HRESULT hr = m_DeviceResources.GetD3D12Device().CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &sm6_0, sizeof(sm6_0));
			ThrowIfFailed(hr, TEXT("Failed to query feature support for shader model 6 with device."));
			if (sm6_0.HighestShaderModel >= D3D_SHADER_MODEL_6_0)
			{
				IE_LOG(Log, TEXT("Shader model 6 is supported."));
			}
			else
			{
				IE_LOG(Error, TEXT("Shader model 6 not supported on selected device."));
			}

			m_FrameResources.Init(&m_DeviceResources.GetD3D12Device());
			hr = m_cbvsrvHeap.Create(&m_DeviceResources.GetD3D12Device(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 70, true);
			ThrowIfFailed(hr, TEXT("Failed to create descriptor heap for shader visible resources."));

			CreateCommandAllocators();


			BeginTrackRenderEvent(&m_DeviceResources.GetGraphicsCommandQueue(), 0, L"D3D12 Context Setup");
			{
				CreateViewport();
				CreateScissorRect();

				// Load Pipelines
				{
					CreateDeferredShadingRS();
					CreateForwardShadingRS();
					CreateBloomPassRS();
					CreateDebugScreenQuadRS();

//#if !IE_PLATFORM_BUILD_XBOX_ONE
//					TryCompiledShaders();
//#endif
					{
						// Create the Geometry Pass and push it to the render stack.
						m_GeometryPass.Create(this, &m_cbvsrvHeap, m_pScenePass_CommandList.Get(), m_pDeferredShadingPass_RS.Get());
						m_RenderPassStack.PushPass(&m_GeometryPass);

						if (m_GraphicsSettings.RayTraceEnabled)
						{
							m_RayTracedShadowPass.Create(this, &m_cbvsrvHeap, m_pRayTracePass_CommandList.Get(), nullptr);
							m_RenderPassStack.PushPass(&m_RayTracedShadowPass);
						}

						// Create the Light Pass and push it to the render stack.
						m_LightPass.Create(this, &m_cbvsrvHeap, m_pScenePass_CommandList.Get(), m_pDeferredShadingPass_RS.Get());
						uint8_t NumGBuffers = m_GeometryPass.GetNumGBuffers();
						for (uint8_t i = 0; i < NumGBuffers; ++i)
							m_LightPass.SetRenderTargetTextureRef(m_GeometryPass.GetGBufferRenderTargetTexture(i));
						m_LightPass.SetSceneDepthTextureRef(m_GeometryPass.GetSceneDepthTexture());
						m_LightPass.SetSkyLightRef(m_pSkyLight);
						m_RenderPassStack.PushPass(&m_LightPass);

						// Create the Sky Pass ad push it to the render stack.
						m_SkyPass.SetSceneDepthTextureRef(m_GeometryPass.GetSceneDepthTexture(), m_GeometryPass.GetSceneDepthCPUHandle());
						m_SkyPass.SetRenderTargetRef(m_LightPass.GetLightPassResult(), m_LightPass.GetLightPassResultCPUHandle());
						m_SkyPass.Create(this, &m_cbvsrvHeap, m_pScenePass_CommandList.Get(), m_pDeferredShadingPass_RS.Get());
						m_SkyPass.SetSkySphereRef(m_pSkySphere);
						m_RenderPassStack.PushPass(&m_SkyPass);

#if BLOOM_ENABLED
						// Create the Bloom Pass ad push it to the render stack.
						m_BloomPass.Create(this, &m_cbvsrvHeap, m_pBloomFirstPass_CommandList.Get(), nullptr);
						m_BloomPass.InitHelpers(m_pBloomFirstPass_CommandList, m_pBloomSecondPass_CommandList.Get());
						m_RenderPassStack.PushPassOverlay(&m_BloomPass);
#endif
						// Create the Post Process Composite Pass ad push it to the render stack.
						m_PostProcessCompositePass.Create(this, &m_cbvsrvHeap, m_pPostEffectsPass_CommandList.Get(), m_pDeferredShadingPass_RS.Get());
						m_PostProcessCompositePass.SetSceneDepthTextureRef(m_GeometryPass.GetSceneDepthTexture());
						m_RenderPassStack.PushPassOverlay(&m_PostProcessCompositePass);
					}
				}

				LoadAssets();
			}
			EndTrackRenderEvent(&m_DeviceResources.GetGraphicsCommandQueue());
		}
		catch (COMException& Ex)
		{
			Platform::CreateMessageBox(Ex.what(), L"Fatal Error!", m_pWindowRef->GetNativeWindow());
			return false;
		}
		return true;
	}

	void Direct3D12Context::LoadAssets()
	{
		auto& DWriteFactory = m_DeviceResources.GetWriteFactory();
		auto& D2DDeviceContext = m_DeviceResources.GetD2DDeviceContext();
		constexpr float FontSize = 20;

		ThrowIfFailed(D2DDeviceContext.CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &m_textBrush), TEXT("Failed to create solid color brush."));
		HRESULT hr = DWriteFactory.CreateTextFormat(
			L"Verdana",
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			FontSize,
			L"en-us",
			&m_textFormat
		);

		m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}

	void Direct3D12Context::Destroy_Impl()
	{
		// TODO: TEMP Move somewhere else
		GraphicsSettings& Settings = GetGraphicsSettings();
		FileSystem::SaveEngineUserSettings(Settings);
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

		DirectX::XMVECTOR InvMatDeterminent;

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
		m_FrameResources.m_CBPerFrame.Data.ScreenSize.x = (float)m_pWindowRef->GetWidth();
		m_FrameResources.m_CBPerFrame.Data.ScreenSize.y = (float)m_pWindowRef->GetHeight();
		m_FrameResources.m_CBPerFrame.SubmitToGPU();

		if (m_GraphicsSettings.RayTraceEnabled) m_RayTracedShadowPass.GetRTHelper()->UpdateCBVs();


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
				TEXT("Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Scene Pass"));

#if SHADOWMAPPING_ENABLED
			ThrowIfFailed(m_pShadowPass_CommandAllocators[IE_D3D12_FrameIndex]->Reset(),
				TEXT("Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Shadow Pass"));
#endif
#if TRANSPARENCYPASS_ENABLED
			ThrowIfFailed(m_pTransparencyPass_CommandAllocators[IE_D3D12_FrameIndex]->Reset(),
				"Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Transparency Pass");
#endif
			ThrowIfFailed(m_pPostEffectsPass_CommandAllocators[IE_D3D12_FrameIndex]->Reset(),
				TEXT("Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Post-Process Pass"));

			ThrowIfFailed(m_pBloomFirstPass_CommandAllocators[IE_D3D12_FrameIndex]->Reset(),
				TEXT("Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Post-Process Pass"));

			ThrowIfFailed(m_pBloomSecondPass_CommandAllocators[IE_D3D12_FrameIndex]->Reset(),
				TEXT("Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Post-Process Pass"));

			if (m_GraphicsSettings.RayTraceEnabled) {
				ThrowIfFailed(m_pRayTracePass_CommandAllocators[IE_D3D12_FrameIndex]->Reset(),
					TEXT("Failed to reset command allocator in Direct3D12Context::OnPreFrameRender for Ray Trace Pass"));
			}
		}

		// Reset Command Lists
		{
			ThrowIfFailed(m_pScenePass_CommandList->Reset(m_pScenePass_CommandAllocators[IE_D3D12_FrameIndex].Get(), nullptr),
				TEXT("Failed to reset command list in Direct3D12Context::OnPreFrameRender for Scene Pass"));

#if SHADOWMAPPING_ENABLED
			ThrowIfFailed(m_pShadowPass_CommandList->Reset(m_pShadowPass_CommandAllocators[IE_D3D12_FrameIndex].Get(), m_pShadowPass_PSO.Get()),
				TEXT("Failed to reset command list in Direct3D12Context::OnPreFrameRender for Shadow Pass"));
#endif 

#if TRANSPARENCYPASS_ENABLED
			ThrowIfFailed(m_pTransparencyPass_CommandList->Reset(m_pTransparencyPass_CommandAllocators[IE_D3D12_FrameIndex].Get(), m_pTransparency_PSO.Get()),
				"Failed to reset command list in Direct3D12Context::OnPreFrameRender for Transparency Pass");
#endif

			ThrowIfFailed(m_pPostEffectsPass_CommandList->Reset(m_pPostEffectsPass_CommandAllocators[IE_D3D12_FrameIndex].Get(), nullptr),
				TEXT("Failed to reset command list in Direct3D12Context::OnPreFrameRender for Transparency Pass"));

#if BLOOM_ENABLED
			ThrowIfFailed(m_pBloomFirstPass_CommandList->Reset(m_pBloomFirstPass_CommandAllocators[IE_D3D12_FrameIndex].Get(), m_pThresholdDownSample_PSO.Get()),
				TEXT("Failed to reset command list in Direct3D12Context::OnPreFrameRender for Transparency Pass"));

			ThrowIfFailed(m_pBloomSecondPass_CommandList->Reset(m_pBloomSecondPass_CommandAllocators[IE_D3D12_FrameIndex].Get(), m_pThresholdDownSample_PSO.Get()),
				TEXT("Failed to reset command list in Direct3D12Context::OnPreFrameRender for Transparency Pass"));
#endif
			if (m_GraphicsSettings.RayTraceEnabled) {
				ThrowIfFailed(m_pRayTracePass_CommandList->Reset(m_pRayTracePass_CommandAllocators[IE_D3D12_FrameIndex].Get(), nullptr),
					TEXT("Failed to reset command list in Direct3D12Context::OnPreFrameRender for Ray Trace Pass"));
			}
		}
	}

	void Direct3D12Context::OnRender_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		// Gather the geometry in the world and send it to the GPU.
		GeometryManager::GatherGeometry();

		// Traverse the render stack and draw the scene.
		for (RenderPass* Pass : m_RenderPassStack)
		{
			Pass->Render(&m_FrameResources);
		}
	}

	void Direct3D12Context::BindShadowPass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		BeginTrackRenderEvent(m_pShadowPass_CommandList.Get(), 0, L"Rendering Shadow Pass");
		{
			ID3D12DescriptorHeap* ppHeaps[] = { m_cbvsrvHeap.pDH.Get() };
			m_pShadowPass_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			m_pShadowPass_CommandList->SetPipelineState(m_pShadowPass_PSO.Get());
			m_pShadowPass_CommandList->SetGraphicsRootSignature(m_pDeferredShadingPass_RS.Get());

			ID3D12Resource* ShadowDepthResources[] = { m_pShadowDepthTexture.Get() };
			ResourceBarrier(m_pShadowPass_CommandList.Get(), ShadowDepthResources, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);

			m_pShadowPass_CommandList->ClearDepthStencilView(m_dsvHeap.hCPU(1), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0xFF, 0, nullptr);
			m_pShadowPass_CommandList->OMSetRenderTargets(0, nullptr, FALSE, &m_dsvHeap.hCPU(1));

			m_pShadowPass_CommandList->RSSetScissorRects(1, &m_ShadowPass_ScissorRect);
			m_pShadowPass_CommandList->RSSetViewports(1, &m_ShadowPass_ViewPort);

			m_pShadowPass_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_FrameResources.m_CBPerFrame.SetAsGraphicsRootConstantBufferView(m_pShadowPass_CommandList.Get(), 1);
			m_FrameResources.m_CBLights.SetAsGraphicsRootConstantBufferView(m_pShadowPass_CommandList.Get(), 2);

			// TODO Shadow pass logic here put this on another thread
			GeometryManager::Render(RenderPassType::RenderPassType_Shadow);
			ResourceBarrier(m_pShadowPass_CommandList.Get(), ShadowDepthResources, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}
		EndTrackRenderEvent(m_pShadowPass_CommandList.Get());
	}

	void Direct3D12Context::OnMidFrameRender_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;
	}

	void Direct3D12Context::OnEditorRender_Impl()
	{
	}

	void Direct3D12Context::BindTransparencyPass()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		BeginTrackRenderEvent(m_pTransparencyPass_CommandList.Get(), 0, "Rendering Transparency Pass");
		{
			ID3D12DescriptorHeap* ppHeaps[] = { m_cbvsrvHeap.pDH.Get() };
			m_pTransparencyPass_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			m_pTransparencyPass_CommandList->SetPipelineState(m_pTransparency_PSO.Get());
			m_pTransparencyPass_CommandList->SetGraphicsRootSignature(m_pForwardShadingPass_RS.Get());

			m_pTransparencyPass_CommandList->RSSetScissorRects(1, &m_DeviceResources.GetClientScissorRect());
			m_pTransparencyPass_CommandList->RSSetViewports(1, &m_DeviceResources.GetClientViewPort());
			m_pTransparencyPass_CommandList->OMSetRenderTargets(1, &m_rtvHeap.hCPU(4), TRUE, &m_dsvHeap.hCPU(0));

			m_pTransparencyPass_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_FrameResources.m_CBPerFrame.SetAsGraphicsRootConstantBufferView(m_pTransparencyPass_CommandList.Get(), 1);
			m_FrameResources.m_CBLights.SetAsGraphicsRootConstantBufferView(m_pTransparencyPass_CommandList.Get(), 2);
			//m_pTransparencyPass_CommandList->SetGraphicsRootDescriptorTable(4, m_cbvsrvHeap.hGPU(4));

			GeometryManager::Render(RenderPassType::RenderPassType_Transparency);
		}
		EndTrackRenderEvent(m_pTransparencyPass_CommandList.Get());
	}

	void Direct3D12Context::DrawDebugScreenQuad()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		BeginTrackRenderEvent(m_pPostEffectsPass_CommandList.Get(), 0, L"Rendering Debug Screen Quad Pass");
		{
			ID3D12DescriptorHeap* ppHeaps[] = { m_cbvsrvHeap.pDH.Get() };
			m_pPostEffectsPass_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			m_pPostEffectsPass_CommandList->OMSetRenderTargets(1, &m_DeviceResources.GetSwapChainRTV(), TRUE, nullptr);

			m_pPostEffectsPass_CommandList->RSSetScissorRects(1, &m_DeviceResources.GetClientScissorRect());
			m_pPostEffectsPass_CommandList->RSSetViewports(1, &m_DeviceResources.GetClientViewPort());

			m_pPostEffectsPass_CommandList->SetPipelineState(m_pDebugScreenQuad_PSO.Get());
			m_pPostEffectsPass_CommandList->SetGraphicsRootSignature(m_pDebugScreenQuad_RS.Get());

			m_FrameResources.m_CBLights.SetAsGraphicsRootConstantBufferView(m_pPostEffectsPass_CommandList.Get(), 1); // Set the light buffer
			m_FrameResources.m_CBPerFrame.SetAsGraphicsRootConstantBufferView(m_pPostEffectsPass_CommandList.Get(), 2); // Set the per frame buffer
			m_pPostEffectsPass_CommandList->SetGraphicsRootDescriptorTable(0, m_cbvsrvHeap.hGPU(7)); // Debug Texture

			m_DebugScreenQuad.OnRender(m_pPostEffectsPass_CommandList);
		}
		EndTrackRenderEvent(m_pPostEffectsPass_CommandList.Get());
	}

	void Direct3D12Context::ExecuteDraw_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

#if SHADOWMAPPING_ENABLED
		ThrowIfFailed(m_pShadowPass_CommandList->Close(), TEXT("Failed to close the command list for D3D 12 context shadow pass."));
#endif
		ThrowIfFailed(m_pScenePass_CommandList->Close(), TEXT("Failed to close command list for D3D 12 context scene pass."));
#if TRANSPARENCYPASS_ENABLED
		ThrowIfFailed(m_pTransparencyPass_CommandList->Close(), TEXT("Failed to close the command list for D3D 12 context transparency pass."));
#endif
		
		ThrowIfFailed(m_pPostEffectsPass_CommandList->Close(), TEXT("Failed to close the command list for D3D 12 context post-process pass."));
		
#if BLOOM_ENABLED
		ThrowIfFailed(m_pBloomFirstPass_CommandList->Close(), TEXT("Failed to close command list for D3D 12 context bloom blur pass."));
		ThrowIfFailed(m_pBloomSecondPass_CommandList->Close(), TEXT("Failed to close command list for D3D 12 context bloom blur pass."));
#endif

		if (m_GraphicsSettings.RayTraceEnabled)
			ThrowIfFailed(m_pRayTracePass_CommandList->Close(), TEXT("Failed to close the command list for D3D 12 context ray trace pass."));

		// Scene Pass
		std::vector<ID3D12CommandList*> pSubmittions;
#if SHADOWMAPPING_ENABLED
		pSubmittions.push_back(m_pShadowPass_CommandList.Get());
#endif
		if (m_GraphicsSettings.RayTraceEnabled)
			pSubmittions.push_back(m_pRayTracePass_CommandList.Get());
		pSubmittions.push_back(m_pScenePass_CommandList.Get());
#if TRANSPARENCYPASS_ENABLED
		pSubmittions.push_back(m_pTransparencyPass_CommandList.Get());
#endif
		m_DeviceResources.GetGraphicsCommandQueue().ExecuteCommandLists((UINT)pSubmittions.size(), pSubmittions.data());

#if BLOOM_ENABLED
		// Bloom Compute
		pSubmittions.clear();
		pSubmittions.push_back(m_pBloomSecondPass_CommandList.Get());
		m_DeviceResources.GetComputeCommandQueue().ExecuteCommandLists((UINT)pSubmittions.size(), pSubmittions.data());
#endif
		// Post Process Pass
		pSubmittions.clear();
		pSubmittions.push_back(m_pPostEffectsPass_CommandList.Get());
		m_DeviceResources.GetGraphicsCommandQueue().ExecuteCommandLists((UINT)pSubmittions.size(), pSubmittions.data());


		BeginTrackRenderEvent(&m_DeviceResources.GetGraphicsCommandQueue(), 0, "Render UI");
		{
			D2D1_SIZE_F rtSize = m_DeviceResources.GetD2DRenderTarget().GetSize();
			D2D1_RECT_F textRect = D2D1::RectF(0, 0, rtSize.width, rtSize.height);
			FrameTimer& GraphicsFrameTimer = Application::Get().GetGraphicsThreadPerfTimer();
			FrameTimer& GameFrameTimer = Application::Get().GetGameThreadPerfTimer();
			std::wstring Text = L"GPU Thread: " + ToString(GraphicsFrameTimer.FPS<int>()) + L" | " + ToString((int)GraphicsFrameTimer.MilliSeconds()) + L"ms\n";
			Text += L"Game Thread: " + ToString(GameFrameTimer.FPS<int>()) + L" | " + ToString((int)GameFrameTimer.MilliSeconds()) + L"ms";

			auto& D2DDeviceContext = m_DeviceResources.GetD2DDeviceContext();
			auto& D3D11On12Device = m_DeviceResources.GetD3D11On12Device();
			auto& D3D11On12DeviceContext = m_DeviceResources.GetD3D11DeviceContext();

			
			// Acquire our wrapped render target resource for the current back buffer.
			ID3D11Resource* WrappedResources[] = {
				&m_DeviceResources.GetWrappedD2DBackBuffer()
			};
			m_DeviceResources.GetD3D11On12Device().AcquireWrappedResources(WrappedResources, _countof(WrappedResources));

			// Render text directly to the back buffer.
			D2DDeviceContext.SetTarget(&m_DeviceResources.GetD2DRenderTarget());
			D2DDeviceContext.BeginDraw();
			D2DDeviceContext.SetTransform(D2D1::Matrix3x2F::Identity());
			D2DDeviceContext.DrawText(
				Text.c_str(),
				(UINT)Text.length(),
				m_textFormat.Get(),
				&textRect,
				m_textBrush.Get()
			);
			ThrowIfFailed(D2DDeviceContext.EndDraw(), TEXT("Failed to end draw for D2D."));

			// Release our wrapped render target resource. Releasing 
			// transitions the back buffer resource to the state specified
			// as the OutState when the wrapped resource was created.
			D3D11On12Device.ReleaseWrappedResources(WrappedResources, _countof(WrappedResources));

			// Flush to submit the 11 command list to the shared command queue.
			D3D11On12DeviceContext.Flush();
		}
		EndTrackRenderEvent(&m_DeviceResources.GetGraphicsCommandQueue());


		m_DeviceResources.WaitForGPU();
	}

	void Direct3D12Context::SwapBuffers_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		UINT PresentFlags = (m_AllowTearing && m_WindowedMode) ? DXGI_PRESENT_ALLOW_TEARING : 0;
		ThrowIfFailed(m_DeviceResources.GetSwapChain().Present(0, PresentFlags), TEXT("Failed to present frame for D3D 12 context."));

		m_DeviceResources.MoveToNextFrame();
	}

	void Direct3D12Context::OnWindowResize_Impl()
	{
		if (!m_IsMinimized)
		{
			if (m_WindowResizeComplete)
			{
				m_WindowResizeComplete = false;
				m_DeviceResources.WaitForGPU();

				UpdateSizeDependentResources();
				static bool FirstLaunch = true;
			}
		}
		m_WindowVisible = !m_IsMinimized;
		m_WindowResizeComplete = true;
	}

	void Direct3D12Context::OnWindowFullScreen_Impl()
	{
#if IE_PLATFORM_BUILD_WIN32

		Win32Window* pWindow = &GetWindowRefAs<Win32Window>();
		HWND pHWND = static_cast<HWND>(m_pWindowRef->GetNativeWindow());

		if (m_FullScreenMode)
		{
			SetWindowLong(pHWND, GWL_STYLE, WS_OVERLAPPEDWINDOW);

			SetWindowPos(
				pHWND,
				HWND_NOTOPMOST,
				pWindow->GetWindowRect().left,
				pWindow->GetWindowRect().top,
				pWindow->GetWindowRect().right - pWindow->GetWindowRect().left,
				pWindow->GetWindowRect().bottom - pWindow->GetWindowRect().top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE
			);
			ShowWindow(pHWND, SW_NORMAL);
		}
		else
		{
			GetWindowRect(pHWND, &pWindow->GetWindowRect());

			SetWindowLong(pHWND, GWL_STYLE, WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

			RECT fullscreenWindowRect;

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
#endif // IE_PLATFORM_BUILD_WIN32
	}

	void Direct3D12Context::OnShaderReload_Impl()
	{
		m_RenderPassStack.ReloadPipelines();
	}

	void Direct3D12Context::SetVertexBuffers_Impl(uint32_t StartSlot, uint32_t NumBuffers, ieVertexBuffer* pBuffers)
	{
		IE_ASSERT(dynamic_cast<D3D12VertexBuffer*>(pBuffers) != nullptr, "A vertex buffer passed to renderer with D3D 12 active must be a \"D3D12VertexBuffer\"");
		m_pActiveCommandList->IASetVertexBuffers(StartSlot, NumBuffers, reinterpret_cast<D3D12VertexBuffer*>(pBuffers)->GetVertexBufferView());
	}

	void Direct3D12Context::SetIndexBuffer_Impl(ieIndexBuffer* pBuffer)
	{
		IE_ASSERT(dynamic_cast<D3D12IndexBuffer*>(pBuffer) != nullptr, "A index buffer passed to renderer with D3D 12 active must be a \"D3D12IndexBuffer\"");
		m_pActiveCommandList->IASetIndexBuffer(&reinterpret_cast<D3D12IndexBuffer*>(pBuffer)->GetIndexBufferView());
	}

	void Direct3D12Context::DrawIndexedInstanced_Impl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation)
	{
		m_pActiveCommandList->DrawIndexedInstanced(IndexCountPerInstance, NumInstances, StartIndexLocation, BaseVertexLoaction, StartInstanceLocation);
	}

	void Direct3D12Context::DrawText_Impl(const wchar_t* Text)
	{
		//m_Text += Text;
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
		return m_RayTracedShadowPass.GetRTHelper()->RegisterBottomLevelASGeometry(pVertexBuffer, pIndexBuffer, NumVerticies, NumIndices, MeshWorldMat);
	}

	void Direct3D12Context::CreateDSVs()
	{
		HRESULT hr = S_OK;
		return;
		ID3D12Device* pDevice = &m_DeviceResources.GetD3D12Device();
		m_dsvHeap.Create(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 2);
		CD3DX12_HEAP_PROPERTIES DefaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

		// Shadow Depth Texture 
		{
			D3D12_CLEAR_VALUE ShadowDepthOptomizedClearValue = {};
			ShadowDepthOptomizedClearValue.Format = m_ShadowMapFormat;
			ShadowDepthOptomizedClearValue.DepthStencil.Depth = 1.0f;
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
				IE_LOG(Error, TEXT("Failed to create comitted resource for depth stencil view"));

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
			D3D12_FILTER_ANISOTROPIC,
			//D3D12_FILTER_MIN_MAG_MIP_LINEAR,
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
		ThrowIfFailed(hr, TEXT("Failed to serialize root signature for D3D 12 context."));

		m_pDeferredShadingPass_RS.Reset();
		hr = m_DeviceResources.GetD3D12Device().CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pDeferredShadingPass_RS));
		ThrowIfFailed(hr, TEXT("Failed to create root signature for D3D 12 context."));
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
		ThrowIfFailed(hr, TEXT("Failed to serialize root signature for D3D 12 context."));

		hr = m_DeviceResources.GetD3D12Device().CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pForwardShadingPass_RS));
		ThrowIfFailed(hr, TEXT("Failed to create root signature for D3D 12 context."));
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
		ThrowIfFailed(hr, TEXT("Failed to serialize root signature for D3D 12 context."));

		hr = m_DeviceResources.GetD3D12Device().CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pBloomPass_RS));
		ThrowIfFailed(hr, TEXT("Failed to create root signature for D3D 12 context."));
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
		ThrowIfFailed(hr, TEXT("Failed to serialize root signature for D3D 12 context."));

		hr = m_DeviceResources.GetD3D12Device().CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pDebugScreenQuad_RS));
		ThrowIfFailed(hr, TEXT("Failed to create root signature for D3D 12 context."));
	}

	void Direct3D12Context::LoadPipelines()
	{
		HRESULT hr = S_OK;

		// Create Shadow Pass
		{
			ComPtr<ID3DBlob> pVertexShader;
			ComPtr<ID3DBlob> pPixelShader;

			//const wchar_t* ExeDirectory = FileSystem::GetWorkingDirectoryW();
			const wchar_t* ExeDirectory = 0;
			std::wstring VertShaderFolder(ExeDirectory);
			VertShaderFolder += L"../Renderer/Shadow_Pass.vertex.cso";
			std::wstring PixelShaderFolder(ExeDirectory);
			PixelShaderFolder += L"../Renderer/Shadow_Pass.pixel.cso";

			/*hr = D3DReadFileToBlob(VertShaderFolder.c_str(), &pVertexShader);
			ThrowIfFailed(hr, "Failed to read Vertex Shader for D3D 12 context.");
			hr = D3DReadFileToBlob(PixelShaderFolder.c_str(), &pPixelShader);
			ThrowIfFailed(hr, "Failed to read Pixel Shader for D3D 12 context.");*/

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

			hr = m_DeviceResources.GetD3D12Device().CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&m_pShadowPass_PSO));
			ThrowIfFailed(hr, TEXT("Failed to create graphics pipeline state for shadow pass in D3D 12 context."));
			m_pShadowPass_PSO->SetName(L"PSO Shadow Pass");
		}

		// Create Debug Screen Quad Pass
		{
			ComPtr<ID3DBlob> pVertexShader;
			ComPtr<ID3DBlob> pPixelShader;

			const wchar_t* ExeDirectory = 0;
			EString VertexShaderFolder(ExeDirectory);
			VertexShaderFolder += L"../Renderer/Screen_Aligned_Quad.vertex.cso";
			EString PixelShaderFolder(ExeDirectory);
			PixelShaderFolder += L"../Renderer/Debug_Screen_Quad.pixel.cso";

			/*hr = D3DReadFileToBlob(VertexShaderFolder.c_str(), &pVertexShader);
			ThrowIfFailed(hr, "Failed to compile Vertex Shader for D3D 12 context.");
			hr = D3DReadFileToBlob(PixelShaderFolder.c_str(), &pPixelShader);
			ThrowIfFailed(hr, "Failed to compile Pixel Shader for D3D 12 context.");*/


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
			descPipelineState.RTVFormats[0] = m_DeviceResources.GetSwapChainBackBufferFormat();
			descPipelineState.SampleDesc.Count = 1;

			hr = m_DeviceResources.GetD3D12Device().CreateGraphicsPipelineState(&descPipelineState, IID_PPV_ARGS(&m_pDebugScreenQuad_PSO));
			ThrowIfFailed(hr, TEXT("Failed to create graphics pipeline state for Debug Screen Quad pass in D3D 12 context."));
			m_pDebugScreenQuad_PSO->SetName(L"PSO Debug Screen Quad Pass");
		}

		// Create Down Sample Pass
		{
			ComPtr<ID3DBlob> pComputeShader;

			const wchar_t* ExeDirectory = 0;
			std::wstring VertexShaderFolder(ExeDirectory);
			VertexShaderFolder += L"../Renderer/Threshold_Down_Sample.compute.cso";

			//hr = D3DReadFileToBlob(VertexShaderFolder.c_str(), &pComputeShader);
			ThrowIfFailed(hr, TEXT("Failed to read compute shader for D3D 12 context"));

			D3D12_SHADER_BYTECODE ComputeShaderBytecode = {};
			ComputeShaderBytecode.BytecodeLength = pComputeShader->GetBufferSize();
			ComputeShaderBytecode.pShaderBytecode = pComputeShader->GetBufferPointer();

			D3D12_COMPUTE_PIPELINE_STATE_DESC PipelineDesc = {};
			PipelineDesc.CS = ComputeShaderBytecode;
			PipelineDesc.pRootSignature = m_pBloomPass_RS.Get();

			hr = m_DeviceResources.GetD3D12Device().CreateComputePipelineState(&PipelineDesc, IID_PPV_ARGS(&m_pThresholdDownSample_PSO));
			ThrowIfFailed(hr, TEXT("Failed to create texture downsampling pipeline for bloom pass."));
			m_pThresholdDownSample_PSO->SetName(L"Bloom Pass PSO");
		}

		// Create Gausian Blur Pass
		{
			ComPtr<ID3DBlob> pComputeShader;

			const wchar_t* ExeDirectory = 0;
			std::wstring ComputeShaderFolder(ExeDirectory);
			ComputeShaderFolder += L"../Renderer/Gaussian_Blur.compute.cso";

			//hr = D3DReadFileToBlob(ComputeShaderFolder.c_str(), &pComputeShader);
			ThrowIfFailed(hr, TEXT("Failed to read compute shader for D3D 12 context"));

			D3D12_SHADER_BYTECODE ComputeShaderBytecode = {};
			ComputeShaderBytecode.BytecodeLength = pComputeShader->GetBufferSize();
			ComputeShaderBytecode.pShaderBytecode = pComputeShader->GetBufferPointer();

			D3D12_COMPUTE_PIPELINE_STATE_DESC PipelineDesc = {};
			PipelineDesc.CS = ComputeShaderBytecode;
			PipelineDesc.pRootSignature = m_pBloomPass_RS.Get();

			hr = m_DeviceResources.GetD3D12Device().CreateComputePipelineState(&PipelineDesc, IID_PPV_ARGS(&m_pGaussianBlur_PSO));
			ThrowIfFailed(hr, TEXT("Failed to create gaussian blur pipeline for bloom pass."));
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
				hr = m_DeviceResources.GetD3D12Device().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pScenePass_CommandAllocators[i]));
				ThrowIfFailed(hr, TEXT("Failed to Scene Pass Create Command Allocator for D3D 12 context"));
				m_pScenePass_CommandAllocators[i]->SetName(L"Scene Pass Command Allocator");
			}

			hr = m_DeviceResources.GetD3D12Device().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pScenePass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pScenePass_CommandList));
			ThrowIfFailed(hr, TEXT("Failed to Scene Pass Create Command List for D3D 12 context"));
			m_pScenePass_CommandList->SetName(L"Scene Pass Command List");

			// Dont close the Scene Pass command list yet. 
			// We'll use it for application initialization.
		}

		// Shadow Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_DeviceResources.GetD3D12Device().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pShadowPass_CommandAllocators[i]));
				ThrowIfFailed(hr, TEXT("Failed to Create Command Allocator for D3D 12 context"));
				m_pShadowPass_CommandAllocators[i]->SetName(L"Shadow Pass Command Allocator");
			}

			hr = m_DeviceResources.GetD3D12Device().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pShadowPass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pShadowPass_CommandList));
			ThrowIfFailed(hr, TEXT("Failed to Create Shadow Pass Command List for D3D 12 context"));
			m_pShadowPass_CommandList->SetName(L"Shadow Pass Command List");

			m_pShadowPass_CommandList->Close();
		}

		// Transprency Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_DeviceResources.GetD3D12Device().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pTransparencyPass_CommandAllocators[i]));
				ThrowIfFailed(hr, TEXT("Failed to Create Command Allocator for D3D 12 context"));
				m_pTransparencyPass_CommandAllocators[i]->SetName(L"Transparency Pass Command Allocator");
			}

			hr = m_DeviceResources.GetD3D12Device().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pTransparencyPass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pTransparencyPass_CommandList));
			ThrowIfFailed(hr, TEXT("Failed to Create Transparency Pass Command List for D3D 12 context"));
			m_pTransparencyPass_CommandList->SetName(L"Transparency Pass Command List");

			m_pTransparencyPass_CommandList->Close();
		}

		// Post-Process Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_DeviceResources.GetD3D12Device().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pPostEffectsPass_CommandAllocators[i]));
				ThrowIfFailed(hr, TEXT("Failed to Create Command Allocator for D3D 12 context"));
				m_pPostEffectsPass_CommandAllocators[i]->SetName(L"Post-Process Pass Command Allocator");
			}

			hr = m_DeviceResources.GetD3D12Device().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pPostEffectsPass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pPostEffectsPass_CommandList));
			ThrowIfFailed(hr, TEXT("Failed to Create Post-Process Pass Command List for D3D 12 context"));
			m_pPostEffectsPass_CommandList->SetName(L"Post-Process Pass Command List");

			m_pPostEffectsPass_CommandList->Close();
		}

		// Gaussian Blur Pass
		{
			// First Pass
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_DeviceResources.GetD3D12Device().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&m_pBloomFirstPass_CommandAllocators[i]));
				ThrowIfFailed(hr, TEXT("Failed to Create Command Allocator for D3D 12 context"));
				m_pBloomFirstPass_CommandAllocators[i]->SetName(L"Gaussian Blur Command Allocator");
			}

			hr = m_DeviceResources.GetD3D12Device().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, m_pBloomFirstPass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pBloomFirstPass_CommandList));
			ThrowIfFailed(hr, TEXT("Failed to Create Gaussian Blur Command List for D3D 12 context"));
			m_pBloomFirstPass_CommandList->SetName(L"Gaussian Blur Command List");

			m_pBloomFirstPass_CommandList->Close();

			// Second Pass
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_DeviceResources.GetD3D12Device().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&m_pBloomSecondPass_CommandAllocators[i]));
				ThrowIfFailed(hr, TEXT("Failed to Create Command Allocator for D3D 12 context"));
				m_pBloomSecondPass_CommandAllocators[i]->SetName(L"Gaussian Blur Command Allocator");
			}

			hr = m_DeviceResources.GetD3D12Device().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, m_pBloomSecondPass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pBloomSecondPass_CommandList));
			ThrowIfFailed(hr, TEXT("Failed to Create Gaussian Blur Command List for D3D 12 context"));
			m_pBloomSecondPass_CommandList->SetName(L"Gaussian Blur Command List");

			m_pBloomSecondPass_CommandList->Close();
		}

		// Ray Trace Pass
		{
			for (int i = 0; i < m_FrameBufferCount; i++)
			{
				hr = m_DeviceResources.GetD3D12Device().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pRayTracePass_CommandAllocators[i]));
				ThrowIfFailed(hr, TEXT("Failed to Create Command Allocator for D3D 12 context"));
				m_pRayTracePass_CommandAllocators[i]->SetName(L"Post-Process Pass Command Allocator");
			}

			hr = m_DeviceResources.GetD3D12Device().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pRayTracePass_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(&m_pRayTracePass_CommandList));
			ThrowIfFailed(hr, TEXT("Failed to Create Post-Process Pass Command List for D3D 12 context"));
			m_pRayTracePass_CommandList->SetName(L"Ray Trace Pass Command List");

			// Dont close the Ray Trace Pass command list yet. 
			// We'll use it for RT pipeline initialization.
		}

		ID3D12CommandList* ppCommandLists[] = {
			m_pShadowPass_CommandList.Get(),
			m_pTransparencyPass_CommandList.Get(),
			m_pPostEffectsPass_CommandList.Get()
		};
		m_DeviceResources.GetGraphicsCommandQueue().ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		m_DeviceResources.IncrementAndSignalFence();
		m_DeviceResources.WaitForGPU();
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
		return;
		uint32_t QuadIndices[] =
		{
			0, 1, 3,
			0, 3, 2
		};

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
		if (m_GraphicsSettings.RayTraceEnabled)
			m_RayTracedShadowPass.GetRTHelper()->GenerateAccelerationStructure();

		m_pScenePass_CommandList->Close();
		m_pRayTracePass_CommandList->Close();

		ID3D12CommandList* ppCommandLists[] = {
			m_pScenePass_CommandList.Get(),
			m_pRayTracePass_CommandList.Get()
		};
		m_DeviceResources.GetGraphicsCommandQueue().ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		m_DeviceResources.IncrementAndSignalFence();

		m_DeviceResources.WaitForGPU();
	}

	void Direct3D12Context::UpdateSizeDependentResources()
	{
		// Re-Create Render Targets
		{
			m_textFormat.Reset();
			m_textBrush.Reset();
			// Resize the Swapchain
			m_DeviceResources.ResizeResources();
			LoadAssets();

			// Resize the buffers in the render stack
			m_RenderPassStack.ReloadBuffers();

			uint8_t NumGBuffers = m_GeometryPass.GetNumGBuffers();
			for (uint8_t i = 0; i < NumGBuffers; ++i)
				m_LightPass.SetRenderTargetTextureRef(m_GeometryPass.GetGBufferRenderTargetTexture(i));
			m_LightPass.SetSceneDepthTextureRef(m_GeometryPass.GetSceneDepthTexture());
			m_LightPass.SetSkyLightRef(m_pSkyLight);

			m_SkyPass.SetSceneDepthTextureRef(m_GeometryPass.GetSceneDepthTexture(), m_GeometryPass.GetSceneDepthCPUHandle());
			m_SkyPass.SetRenderTargetRef(m_LightPass.GetLightPassResult(), m_LightPass.GetLightPassResultCPUHandle());
			m_SkyPass.SetSkySphereRef(m_pSkySphere);

			if (m_GraphicsSettings.RayTraceEnabled)
				m_RayTracedShadowPass.GetRTHelper()->ReCreateOutputBuffer();

			m_PostProcessCompositePass.SetSceneDepthTextureRef(m_GeometryPass.GetSceneDepthTexture());

#if BLOOM_ENABLED
			m_BloomPass.ResizeHelperBuffers();
#endif
			BOOL FullScreenState;
			m_DeviceResources.GetSwapChain().GetFullscreenState(&FullScreenState, nullptr);
			m_WindowedMode = !FullScreenState;
			m_DeviceResources.SetFrameIndex(m_DeviceResources.GetSwapChain().GetCurrentBackBufferIndex());
		}

		// Recreate Camera Projection Matrix
		{
			if (!m_pWorldCameraRef->GetIsOrthographic()) {
				m_pWorldCameraRef->SetPerspectiveProjectionValues(m_pWorldCameraRef->GetFOV(), m_pWindowRef->GetAspectRatio(), m_pWorldCameraRef->GetNearZ(), m_pWorldCameraRef->GetFarZ());
			}
		}
	}

	void Direct3D12Context::ResourceBarrier(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource** pResources, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter, uint32_t NumBarriers)
	{
		constexpr int MaxBarrierTransitions = 8;

		IE_ASSERT(NumBarriers <= MaxBarrierTransitions, "Can only transition 8 resources at a time.");
		IE_ASSERT(pResources, "pResources cannot be nullptr.");

		// Batching transitions is much faster for the GPU than one at a time.
		D3D12_RESOURCE_BARRIER Barriers[MaxBarrierTransitions];
		ZeroMemory(Barriers, sizeof(D3D12_RESOURCE_BARRIER) * MaxBarrierTransitions);

		for (uint8_t i = 0; i < NumBarriers; ++i)
		{
			Barriers[i].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			Barriers[i].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			Barriers[i].Transition.pResource = pResources[i];
			Barriers[i].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			Barriers[i].Transition.StateBefore = StateBefore;
			Barriers[i].Transition.StateAfter = StateAfter;
		}
		pCommandList->ResourceBarrier(NumBarriers, Barriers);
	}

}