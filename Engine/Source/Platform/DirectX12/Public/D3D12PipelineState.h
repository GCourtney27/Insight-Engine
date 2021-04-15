#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/IPipelineState.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class INSIGHT_API D3D12PipelineState : public IPipelineState
			{
			public:
				D3D12PipelineState();
				virtual ~D3D12PipelineState();
				
				virtual void* GetNativePSO() override { return RCast<void*>(m_pID3D12PipelineState); }

				virtual void Initialize(const PipelineStateDesc& Desc) override;


			protected:
				ID3D12PipelineState* m_pID3D12PipelineState;
			};
		}
	}
}
