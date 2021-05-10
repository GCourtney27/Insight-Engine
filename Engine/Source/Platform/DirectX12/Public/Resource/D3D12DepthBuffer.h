#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/Resource/IDepthBuffer.h"
#include "Platform/DirectX12/Public/Resource/D3D12PixelBuffer.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class INSIGHT_API D3D12DepthBuffer : public IDepthBuffer, public D3D12PixelBuffer
			{
			public:
				D3D12DepthBuffer(float ClearDepth = 1.0f, uint8_t ClearStencil = 0)
					: IDepthBuffer(ClearDepth, ClearStencil)
				{
					m_hDSV[0].ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
					m_hDSV[1].ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
					m_hDSV[2].ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
					m_hDSV[3].ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
					m_hDepthSRV.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
					m_hStencilSRV.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
				}
				virtual ~D3D12DepthBuffer() {}

				virtual void Create(const FString& Name, UInt32 Width, UInt32 Height, EFormat Format) override;

				// Get pre-created CPU-visible descriptor handles
				const D3D12_CPU_DESCRIPTOR_HANDLE& GetDSV() const { return m_hDSV[0]; }
				const D3D12_CPU_DESCRIPTOR_HANDLE& GetDSV_DepthReadOnly() const { return m_hDSV[1]; }
				const D3D12_CPU_DESCRIPTOR_HANDLE& GetDSV_StencilReadOnly() const { return m_hDSV[2]; }
				const D3D12_CPU_DESCRIPTOR_HANDLE& GetDSV_ReadOnly() const { return m_hDSV[3]; }
				const D3D12_CPU_DESCRIPTOR_HANDLE& GetDepthSRV() const { return m_hDepthSRV; }
				const D3D12_CPU_DESCRIPTOR_HANDLE& GetStencilSRV() const { return m_hStencilSRV; }

			protected:

				void CreateDerivedViews(IDevice* Device, EFormat Format);

				D3D12_CPU_DESCRIPTOR_HANDLE m_hDSV[4];
				D3D12_CPU_DESCRIPTOR_HANDLE m_hDepthSRV;
				D3D12_CPU_DESCRIPTOR_HANDLE m_hStencilSRV;
			};
		}
	}
}