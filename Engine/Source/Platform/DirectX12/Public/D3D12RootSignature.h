#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/IRootSignature.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class INSIGHT_API D3D12RootSignature : public IRootSignature
			{
			public:
				D3D12RootSignature();
				virtual ~D3D12RootSignature();

				virtual void* GetNativeSignature() { return RCast<void*>(m_pID3D12RootSignature); }

				virtual void Initialize(const RootSignatureDesc& Desc);

			protected:
				ID3D12RootSignature* m_pID3D12RootSignature;
			};
		}
	}
}
