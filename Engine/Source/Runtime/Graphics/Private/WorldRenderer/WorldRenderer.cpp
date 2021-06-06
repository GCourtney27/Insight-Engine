// Copyright Insight Interactive. All Rights Reserved.
#include <Engine_pch.h>

#include "Graphics/Public/WorldRenderer/WorldRenderer.h"

#include "Core/Public/ieObject/Components/ieCameraComponent.h"
#include "Core/Public/ieObject/ieWorld.h"
#include "Graphics/Public/GeometryGenerator.h"

//#ifdef IE_WITH_D3D12
#include "Platform/DirectX12/Public/D3D12RenderContextFactory.h"
//#endif

namespace Insight
{

	void WorldRenderer::Initialize(std::shared_ptr<Window> pWindow, Graphics::ERenderBackend api)
	{
		IE_ASSERT(pWindow.get() != NULL); // Trying to create a renderer with no window to render to!
		pWindow = pWindow;

		// Setup renderer
		switch (api)
		{
		case Graphics::RB_Direct3D12:
		{
			IE_LOG(Log, TEXT("Initializing graphics context with D3D12 rendering backend."));

			ScopedMilliSecondTimer(TEXT("Render backend init"));
			Graphics::DX12::D3D12RenderContextFactory Factory;
			Factory.CreateContext(&m_pRenderContext, pWindow);
		}
		break;
		default:
			break;
		}

		ScopedMilliSecondTimer(TEXT("Render context resource create"));

		// TODO: View and Scissor rects should be controlled elsewhere
		m_SceneViewPort.TopLeftX = 0.f;
		m_SceneViewPort.TopLeftY = 0.f;
		m_SceneViewPort.MinDepth = 0.f;
		m_SceneViewPort.MaxDepth = 1.f;
		m_SceneViewPort.Width = (float)pWindow->GetWidth();
		m_SceneViewPort.Height = (float)pWindow->GetHeight();

		m_SceneScissorRect.Left = 0;
		m_SceneScissorRect.Top = 0;
		m_SceneScissorRect.Right = pWindow->GetWidth();
		m_SceneScissorRect.Bottom = pWindow->GetHeight();

		m_pSwapChain = m_pRenderContext->GetSwapChain();
		Color ClearColor(0.f, .3f, .3f);
		m_pSwapChain->SetClearColor(ClearColor);

		// TODO: Set from somewhere else.
		pWindow->SetWindowMode(EWindowMode::WM_Windowed);

		m_DeferedShadingPipeline.Initialize(pWindow->GetDimensions(), Graphics::g_pDevice, m_pRenderContext->GetSwapChain()->GetDesc().Format);
		m_PostProcessUber.Initialize(pWindow->GetDimensions(), Graphics::g_pDevice);
		m_SkyPass.Initialize(Graphics::g_pDevice, m_pRenderContext->GetSwapChain()->GetDesc().Format, m_DeferedShadingPipeline.m_GeometryPass.GetDepthFormat());
		// m_pRenderContext->GetSwapChain()->GetDesc().Format
		// m_DeferedShadingPipeline.m_LightPass.GetPassResultFormat()

		CreateResources();

		pScreenQuad = GeometryGenerator::GenerateScreenAlignedQuadMesh();

		IE_LOG(Log, TEXT("Graphics context initialized."));
	}


	void WorldRenderer::CreateResources()
	{
		//
		// Init constant buffers
		//
		Graphics::g_pConstantBufferManager->CreateConstantBuffer(TEXT("Scene Constants"), &m_pSceneConstantBuffer, sizeof(SceneConstants));

		Graphics::g_pConstantBufferManager->CreateConstantBuffer(TEXT("Scene Lights"), &m_pLightConstantBuffer, sizeof(SceneLights));

	}

	void WorldRenderer::SetCommonState(Graphics::ICommandContext& CmdContext)
	{
		// Set View and Scissor
		//
		CmdContext.RSSetViewPorts(1, &m_SceneViewPort);
		CmdContext.RSSetScissorRects(1, &m_SceneScissorRect);

		// Set Constant Buffers
		//
		ieCameraComponent& CurrentCamera = *(m_pWorld->GetCurrentSceneRenderCamera());
		SceneConstants* pData = m_pSceneConstantBuffer->GetBufferPointer<SceneConstants>();
		pData->ViewMat		= CurrentCamera.GetViewMatrix().Transpose();
		pData->ProjMat		= CurrentCamera.GetProjectionMatrix().Transpose();
		pData->ViewMat.Invert(pData->InverseViewMat);
		pData->ProjMat.Invert(pData->InverseProjMat);
		pData->CameraPos	= CurrentCamera.GetTransform().GetAbsoluteWorldPosition();
		pData->WorldTime	= (float)m_GFXTimer.Seconds();
		pData->CameraFarZ	= CurrentCamera.GetFarZ();
		pData->CameraNearZ	= CurrentCamera.GetNearZ();
		CmdContext.SetGraphicsConstantBuffer(kSceneConstants, m_pSceneConstantBuffer);

		SceneLights* pLights = m_pLightConstantBuffer->GetBufferPointer<SceneLights>();
		pLights->PointLights[0].Position	= FVector4(45.f, 0.f, 0.f, 1.f);
		pLights->PointLights[0].Color		= FVector4(1.f, 0.f, 0.f, 1.f);
		pLights->PointLights[0].Brightness	= 2000.f;
		pLights->PointLights[1].Position	= FVector4(-45.f, 0.f, 0.f, 1.f);
		pLights->PointLights[1].Color		= FVector4(0.f, 0.f, 1.f, 1.f);
		pLights->PointLights[1].Brightness	= 2000.f;
		pLights->PointLights[2].Position	= FVector4(0.f, 0.f, 45.f, 1.f);
		pLights->PointLights[2].Color		= FVector4(0.f, 1.f, 0.f, 1.f);
		pLights->PointLights[2].Brightness	= 2000.f;
		pLights->NumPointLights = 3.f;
		CmdContext.SetGraphicsConstantBuffer(kLights, m_pLightConstantBuffer);
	}

	void WorldRenderer::Render()
	{
		m_GFXTimer.Tick();

		{
			m_pRenderContext->PreFrame();

			// Render stuff
			Graphics::ICommandContext& CmdContext = Graphics::ICommandContext::Begin(L"Scene Pass");
			{
				// Transition Swapchain for Final Presentation
				Graphics::IColorBuffer* pSwapChainBackBuffer = m_pSwapChain->GetColorBufferForCurrentFrame();
				Graphics::IGPUResource& BackSwapChainBuffer = *DCast<Graphics::IGPUResource*>(pSwapChainBackBuffer);
				CmdContext.TransitionResource(BackSwapChainBuffer, Graphics::RS_RenderTarget);


				// Bind the texture heap
				//
				CmdContext.SetDescriptorHeap(Graphics::RHT_CBV_SRV_UAV, Graphics::g_pTextureHeap);

				// Render the Geometry Pass
				//
				m_DeferedShadingPipeline.m_GeometryPass.Set(CmdContext, m_SceneScissorRect);


				// Set common state
				//
				SetCommonState(CmdContext);


				// Draw world
				//
				RenderStaticMeshGeometry(CmdContext);
				RenderSkinnedMeshGeometry(CmdContext);

				m_DeferedShadingPipeline.m_GeometryPass.UnSet(CmdContext);

				CmdContext.ClearColorBuffer(*pSwapChainBackBuffer, m_SceneScissorRect);
				const Graphics::IColorBuffer* pRTs[] = {
					pSwapChainBackBuffer,
				};
				CmdContext.OMSetRenderTargets(1, pRTs, NULL);
				CmdContext.RSSetViewPorts(1, &m_SceneViewPort);
				CmdContext.RSSetScissorRects(1, &m_SceneScissorRect);


				// Render the Light Pass
				//
				m_DeferedShadingPipeline.m_LightPass.Set(CmdContext, m_SceneScissorRect, m_DeferedShadingPipeline.m_GeometryPass);
				SetCommonState(CmdContext);
				CmdContext.SetPrimitiveTopologyType(Graphics::PT_TiangleList);
				CmdContext.BindVertexBuffer(0, pScreenQuad->GetVertexBuffer());
				CmdContext.BindIndexBuffer(pScreenQuad->GetIndexBuffer());
				CmdContext.DrawIndexedInstanced(pScreenQuad->GetNumIndices(), 1, 0, 0, 0);
				m_DeferedShadingPipeline.m_LightPass.UnSet(CmdContext);

				m_SkyPass.Set(CmdContext, *pSwapChainBackBuffer, *m_DeferedShadingPipeline.m_GeometryPass.GetSceneDepthBuffer());
				CmdContext.SetDescriptorHeap(Graphics::RHT_CBV_SRV_UAV, Graphics::g_pTextureHeap);
				SetCommonState(CmdContext);
				m_SkyPass.UnSet(CmdContext, *m_DeferedShadingPipeline.m_GeometryPass.GetSceneDepthBuffer());

				// Present
				CmdContext.TransitionResource(BackSwapChainBuffer, Graphics::RS_Present);
			}
			CmdContext.Finish();

			m_pRenderContext->SubmitFrame();
			m_pRenderContext->Present();
		}

	}
	void WorldRenderer::RenderStaticMeshGeometry(Graphics::ICommandContext& GfxContext)
	{
		std::vector<ieActor*>& Actors = m_pWorld->GetAllActors();
		for (UInt32 i = 0; i < Actors.size(); ++i)
		{
			Actors[i]->Render(GfxContext);
		}
		//IE_LOG(Log, TEXT("Rendering static mesh geometry."));
		//m_StaticMeshRenderer.Render(GfxContext);
	}
}