#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/RenderCore.h"


namespace Insight
{
	class ieWorld;

	class INSIGHT_API DeferredShadingPipeline
	{
	public:
		DeferredShadingPipeline() {}
		~DeferredShadingPipeline() {}

		void Initialize(ieWorld* pWorld, FVector2 RenderResolution, Graphics::IDevice* pDevice, Graphics::EFormat SwapchainFormatTEMP);
		void UnInitialize();

		enum EGBuffers
		{
			GB_Albedo = 0,
			GB_Normal = 1,
			GB_Position = 2,
			
			GB_NumBuffers = 3,
		};

		
		struct GeometryPass
		{
			void Initialize(Graphics::IDevice* pDevice, const FVector2& RenderResolution);
			void UnInitialize();

			void Set(Graphics::ICommandContext& GfxContext, Graphics::Rect Viewrect);
			void UnSet(Graphics::ICommandContext& GfxContext);

			inline Graphics::IColorBuffer& GetGBufferColorBuffer(EGBuffers GBuffer) const
			{
				IE_ASSERT(GBuffer < GB_NumBuffers); // Index out of range!
				return *m_RenderTargets[GBuffer];
			}

			inline Graphics::IDepthBuffer* GetSceneDepthBuffer() const
			{
				return m_pDepthBuffer;
			}

		protected:
			// Pipeline
			Graphics::IRootSignature* m_pRS = NULL;
			Graphics::IPipelineState* m_pPSO = NULL;
			// Resources
			Graphics::IColorBuffer* m_RenderTargets[GB_NumBuffers];
			Graphics::IDepthBuffer* m_pDepthBuffer;
			Graphics::IGPUResource* m_pDepthBufferGPUResource;
		};

		struct LightPass
		{
			void Initialize(Graphics::IDevice* pDevice, const FVector2& RenderResolution, Graphics::EFormat SwapchainFormatTEMP);
			void UnInitialize();

			void Set(Graphics::ICommandContext& GfxContext, Graphics::Rect Viewrect, const DeferredShadingPipeline::GeometryPass& GeometryPass);
			void UnSet(Graphics::ICommandContext& GfxContext);
			

		protected:
			// Pipeline
			Graphics::IRootSignature* m_pRS = NULL;
			Graphics::IPipelineState* m_pPSO = NULL;
			// Resources
			Graphics::IColorBuffer* m_LightPassResult = NULL;
		};

		GeometryPass m_GeometryPass;
		LightPass m_LightPass;
	};
}
