#pragma once

#include "EngineDefines.h"

#include "Graphics/Public/RenderCore.h"
#include "Graphics/Public/ResourceManagement/ModelManager.h"
#include "Graphics/Public/ResourceManagement/ITextureManager.h"


namespace Insight
{
	class INSIGHT_API SkyboxPass
	{
	public:
		SkyboxPass();
		~SkyboxPass();
		
		void Initialize(Graphics::IDevice* pDevice, Graphics::EFormat RenderTargetFormat, Graphics::EFormat DepthBufferFormat);

		void Set(Graphics::ICommandContext& GfxContext, Graphics::IColorBuffer& RenderTarget, Graphics::IDepthBuffer& DepthBuffer);
		void UnSet(Graphics::ICommandContext& GfxContext, Graphics::IDepthBuffer& DepthBuffer);

	private:
		StaticMeshGeometryRef m_SkyGeometry;
		Graphics::ITextureRef m_SkyDiffuse;

		Graphics::IRootSignature* m_pRS;
		Graphics::IPipelineState* m_pPSO;
	};
}
