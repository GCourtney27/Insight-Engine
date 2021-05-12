// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/RenderCore.h"

#include "Runtime/Core/Public/ECS/EntityAdmin/EntityAdminFwd.h"
#include "Runtime/Core/Public/ECS/EntityAdmin/SystemFwd.h"

#include "Runtime/Core/Public/Window.h"
#include "Runtime/Core/Public/Utility/FrameTimer.h"
#include "Runtime/Core/Public/ieObject/Components/ieStaticMeshComponent.h"
#include "Runtime/Graphics/Public/WorldRenderer/Common.h"

namespace Insight
{
	class ieWorld;

	template <typename MeshRenderType>
	class INSIGHT_API GeometryRenderer : public ECS::GenericSystem<MeshRenderType>
	{
	public:
		GeometryRenderer(const ECS::EntityAdmin& EntityAdmin, const char* DebugName)
			: GenericSystem(EntityAdmin, DebugName)
		{
		}
		virtual ~GeometryRenderer() {}


		virtual void Execute() override
		{
		}

		void Render(Graphics::ICommandContext& GfxContext)
		{
			std::vector<MeshRenderType>& Components = GetRawComponentData();

			// TODO: 1) Sort the scene
			// TODO: 2) Render Opaque
			// TODO: 3) Render Translucent
			for (MeshRenderType& Mesh : Components)
			{
				Mesh.Draw(GfxContext);
			}
		}

	protected:

	};

	class INSIGHT_API WorldRenderer
	{
	public:
		WorldRenderer(ieWorld* pWorld, const ECS::EntityAdmin& EntityAdmin)
			: m_pRenderContext(NULL)
			, m_StaticMeshRenderer(EntityAdmin, "Static Mesh Renderer")
			, m_pWorld(pWorld)
			//, m_SkinnedMeshRenderer(EntityAdmin)
		{
		}

		virtual ~WorldRenderer()
		{
		}

		/*
			Returns the time in milliseonds between each frame.
		*/
		inline float GetFrameTime() const;

		void Initialize(std::shared_ptr<Window> pWindow, Graphics::ERenderBackend api);

		void Render();

		void RenderStaticMeshGeometry(Graphics::ICommandContext& GfxContext)
		{
			//IE_LOG(Log, TEXT("Rendering static mesh geometry."));
			m_StaticMeshRenderer.Render(GfxContext);
		}

		void RenderSkinnedMeshGeometry(Graphics::ICommandContext& GfxContext)
		{
			//m_SkinnedMeshRenderer.Execute();
		}

	private:
		void CreateResources();

	protected:
		Graphics::IRenderContext* m_pRenderContext;
		ieWorld* m_pWorld;
		GeometryRenderer<ieStaticMeshComponent> m_StaticMeshRenderer;
		//GeometryRenderer<ieSkinnedMeshComponent> m_SkinnedMeshRenderer;
		std::shared_ptr<Window> m_pWindow;
		Graphics::ViewPort m_SceneViewPort;
		Graphics::Rect m_SceneScissorRect;

		FrameTimer m_GFXTimer;

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

	inline float WorldRenderer::GetFrameTime() const
	{
		return m_GFXTimer.DeltaTime();
	}
}
