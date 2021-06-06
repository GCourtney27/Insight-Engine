#pragma once

#include "EngineDefines.h"
#include <Runtime/Graphics/Public/GraphicsCore.h>

#include "Graphics/Public/IRootSignature.h"


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

				virtual void* GetNativeSignature() override { return RCast<void*>(m_pID3D12RootSignature); }

				virtual void DestroyAll() override;

				virtual void Initialize(const RootSignatureDesc& Desc) override;
				virtual void Finalize(const FString& name, ERootSignatureFlags Flags) override;

			protected:
				ID3D12RootSignature* m_pID3D12RootSignature;
			};
		}
	}
}
