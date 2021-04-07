#pragma once

#include <Runtime/Core.h>

// Insight
#include "Runtime/Graphics/Public/IRenderContext.h"


namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{

			class INSIGHT_API D3D12RenderContext : public IRenderContext
			{
			public:
				D3D12RenderContext() = default;
				virtual ~D3D12RenderContext() = default;

				virtual void Initialize() override;
				virtual void UnInitialize() override;

				virtual void PreFrame() override;
				virtual void SubmitFrame() override;

			protected:

			};
		}
	}
}