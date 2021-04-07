#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/Resource/IColorBuffer.h"
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
				D3D12ColorBuffer() = default;
				virtual ~D3D12ColorBuffer() = default;
				
				virtual void CreateFromSwapChain(IDevice* pDevice, const EString& Name, void* pResource) override;
				virtual void Create(IDevice* pDevice, const EString& Name, UInt32 Width, UInt32 Height, UInt32 NumMips, ETextureFormat Format) override;

				FORCE_INLINE D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() const { return m_SRVHandle; }
				FORCE_INLINE D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() const { return m_RTVHandle; }

			protected:
				D3D12_CPU_DESCRIPTOR_HANDLE m_SRVHandle;
				D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandle;
				D3D12_CPU_DESCRIPTOR_HANDLE m_UAVHandle[12];
			};
		}

	}
}
