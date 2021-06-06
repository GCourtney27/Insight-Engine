#pragma once

#include "EngineDefines.h"

#include "Graphics/Public/RenderCore.h"


namespace Insight
{
	class ieWorld;

	class INSIGHT_API DeferredShadingPipeline
	{
	public:
		DeferredShadingPipeline() = default;
		~DeferredShadingPipeline() = default;

		void Initialize(FVector2 RenderResolution, Graphics::IDevice* pDevice, Graphics::EFormat SwapchainFormatTEMP);
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

			inline Graphics::IColorBuffer& operator[](size_t Index)
			{
				return GetGBufferColorBuffer((EGBuffers)Index);
			}

			inline Graphics::IColorBuffer& GetGBufferColorBuffer(EGBuffers GBuffer) const
			{
				IE_ASSERT(GBuffer < GB_NumBuffers && GBuffer > -1); // Index out of range!
				return ( *m_RenderTargets[GBuffer] );
			}

			inline Graphics::IDepthBuffer* GetSceneDepthBuffer() const
			{
				return m_pDepthBuffer;
			}

			inline Graphics::EFormat GetDepthFormat() const
			{
				return DCast<Graphics::IPixelBuffer*>(m_pDepthBuffer)->GetFormat();
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
			
			inline Graphics::IColorBuffer* GetLightPassResult() const
			{
				return m_pPassResult;
			}
			
			inline Graphics::EFormat GetPassResultFormat() const
			{
				return DCast<Graphics::IPixelBuffer*>(m_pPassResult)->GetFormat();
			}

		protected:
			// Pipeline
			Graphics::IRootSignature* m_pRS = NULL;
			Graphics::IPipelineState* m_pPSO = NULL;
			// Resources
			Graphics::IColorBuffer* m_pPassResult = NULL;
		};

		GeometryPass m_GeometryPass;
		LightPass m_LightPass;
	};
}
