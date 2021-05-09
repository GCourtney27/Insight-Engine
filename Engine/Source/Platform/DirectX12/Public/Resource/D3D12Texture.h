#pragma once

#include <Runtime/Core.h>
#include <Runtime/Graphics/Public/GraphicsCore.h>

#include "Runtime/Graphics/Public/Resource/ITexture.h"
#include "Platform/DirectX12/Public/Resource/D3D12GPUResource.h"
#include "Runtime/Graphics//Public/IDescriptorHeap.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class INSIGHT_API D3D12Texture : public ITexture, public D3D12GPUResource
			{
			public:
				D3D12Texture() 
				{
					m_hCpuDescriptorHandle.ptr = IE_INVALID_GPU_ADDRESS;
				}
				virtual ~D3D12Texture() {}

				const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV() const { return m_hCpuDescriptorHandle; }

				DescriptorHandle GetShaderVisibleDescriptorHandle() const { return m_DescriptorHandle; }

				virtual void Create2D(UInt64 RowPitchBytes, UInt64 Width, UInt64 Height, EFormat Format, const void* InitData) override;
				virtual void CreateCube(UInt64 RowPitchBytes, UInt64 Width, UInt64 Height, EFormat Format, const void* InitialData) override;
				virtual void Destroy() override;
				
			protected:
				void AssociateWithShaderVisibleHeap();

				D3D12_CPU_DESCRIPTOR_HANDLE m_hCpuDescriptorHandle;
				DescriptorHandle m_DescriptorHandle;
			};
		}
	}
}
