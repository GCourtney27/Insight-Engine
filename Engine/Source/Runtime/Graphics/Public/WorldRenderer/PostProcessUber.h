#pragma once

#include <Runtime/Core.h>
#include "Runtime/Graphics/Public/RenderCore.h"

namespace Insight
{
	class INSIGHT_API PostProcessUber
	{
	public:
		PostProcessUber() = default;
		~PostProcessUber() = default;
		
		void Initialize(FVector2 RenderResolution, Graphics::IDevice* pDevice);

		void Set(Graphics::ICommandContext& GfxContext, Graphics::Rect Viewrect, const Graphics::IColorBuffer& Input);
		void UnSet(Graphics::ICommandContext& GfxContext);

	protected:
		// Pipeline
		Graphics::IRootSignature* m_pRS = NULL;
		Graphics::IPipelineState* m_pPSO = NULL;
		// Resources
		Graphics::IColorBuffer* m_pPassResult = NULL;
	};
}
