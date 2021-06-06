// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include "EngineDefines.h"

#include "Graphics/Public/RenderCore.h"

#include "Core/Public/ieObject/ieActor.h"
#include "Core/Public/ECS/EntityAdmin/SystemFwd.h"
#include "Core/Public/ECS/EntityAdmin/EntityAdminFwd.h"

#include "Core/Public/Window.h"
#include "Core/Public/Utility/FrameTimer.h"
#include "Graphics/Public/WorldRenderer/RendererCommon.h"
#include "Core/Public/ieObject/Components/ieStaticMeshComponent.h"

#include "Graphics/Public/WorldRenderer/SkyBoxPass.h"
#include "Graphics/Public/WorldRenderer/PostProcessUber.h"
#include "Graphics/Public/WorldRenderer/DeferredRenderer/DeferredRenderer.h"

namespace Insight
{
	class ieWorld;

	template <typename MeshRenderType>
	class INSIGHT_API GeometryRenderer /*: public ECS::GenericSystem<MeshRenderType>*/
	{
		ieWorld* m_pWorldRef;
	public:
		GeometryRenderer(ieWorld* pWorld, const ECS::EntityAdmin& EntityAdmin, const char* DebugName)
			//: GenericSystem(EntityAdmin, DebugName)
			: m_pWorldRef(pWorld)
		{
		}
		virtual ~GeometryRenderer() {}


		virtual void Execute() override
		{
		}

		void Render(Graphics::ICommandContext& GfxContext)
		{
#if IE_CACHEOPTIMIZED_ECS_ENABLED
			std::vector<MeshRenderType>& Components = GetRawComponentData();

			// TODO: 1) Sort the scene
			// TODO: 2) Render Opaque
			// TODO: 3) Render Translucent
			for (MeshRenderType& Mesh : Components)
			{
				Mesh.Draw(GfxContext);
			}
#endif
		}

	protected:

	};

	class INSIGHT_API WorldRenderer
	{
	public:
		WorldRenderer(ieWorld* pWorld, const ECS::EntityAdmin& EntityAdmin)
			: m_pRenderContext(NULL)
			//, m_StaticMeshRenderer(EntityAdmin, "Static Mesh Renderer")
			, m_pWorld(pWorld)
			//, m_SkinnedMeshRenderer(EntityAdmin)
		{
		}

		virtual ~WorldRenderer()
		{
			SAFE_DELETE_PTR(m_pRenderContext);
		}

		/*
			Returns the time in milliseonds between each frame.
		*/
		inline float GetFrameRenderTime() const;

		void Initialize(std::shared_ptr<Window> pWindow, Graphics::ERenderBackend api);

		void Render();

		void RenderStaticMeshGeometry(Graphics::ICommandContext& GfxContext);
		

		void RenderSkinnedMeshGeometry(Graphics::ICommandContext& GfxContext)
		{
			//m_SkinnedMeshRenderer.Execute();
		}

	private:
		void CreateResources();
		void SetCommonState(Graphics::ICommandContext& CmdContext);

	protected:
		Graphics::IRenderContext* m_pRenderContext;
		ieWorld* m_pWorld;
		//GeometryRenderer<ieStaticMeshComponent> m_StaticMeshRenderer;
		//GeometryRenderer<ieSkinnedMeshComponent> m_SkinnedMeshRenderer;
		Graphics::ViewPort m_SceneViewPort;
		Graphics::Rect m_SceneScissorRect;

		FrameTimer m_GFXTimer;

		DeferredShadingPipeline m_DeferedShadingPipeline;
		PostProcessUber m_PostProcessUber;
		SkyboxPass m_SkyPass;
		StaticMeshGeometryRef pScreenQuad;

		// 
		// Render Resources
		//
		Graphics::ISwapChain* m_pSwapChain;
		Graphics::IColorBuffer* m_pSceneBuffer;
		Graphics::IDepthBuffer* m_pDepthBuffer;
		// Pipeline
		Graphics::IRootSignature* m_pCommonRS = NULL;
		Graphics::IPipelineState* m_pScenePassPSO = NULL;
		// Constant buffers
		Graphics::IConstantBuffer* m_pSceneConstantBuffer = NULL;
		Graphics::IConstantBuffer* m_pLightConstantBuffer = NULL;

	};

	//
	// Inline funciton implementations
	//

	inline float WorldRenderer::GetFrameRenderTime() const
	{
		return m_GFXTimer.DeltaTime();
	}
}
