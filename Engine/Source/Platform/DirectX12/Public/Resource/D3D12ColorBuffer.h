#pragma once

#include "EngineDefines.h"

#include "Graphics/Public/Resource/IColorBuffer.h"
#include "Platform/DirectX12/Public/Resource/D3D12PixelBuffer.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class INSIGHT_API D3D12ColorBuffer : public IColorBuffer, public D3D12PixelBuffer
			{
			public:
				D3D12ColorBuffer();
				virtual ~D3D12ColorBuffer();
				
				virtual void CreateFromSwapChain(IDevice* pDevice, const FString& Name, void* pResource) override;
				virtual void Create(IDevice* pDevice, const FString& Name, UInt32 Width, UInt32 Height, UInt32 NumMips, EFormat Format) override;

				FORCEINLINE D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() const { return m_SRVHandle; }
				FORCEINLINE D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() const { return m_RTVHandle; }

			protected:
				virtual void CreateDerivedViews(IDevice* pDevice, EFormat Format, UInt32 ArraySize, UInt32 NumMips) override;

			protected:
				D3D12_CPU_DESCRIPTOR_HANDLE m_SRVHandle;
				D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandle;
				D3D12_CPU_DESCRIPTOR_HANDLE m_UAVHandle[12];
			};
		}

	}
}
