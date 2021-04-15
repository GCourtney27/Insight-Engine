#pragma once
#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/GraphicsCore.h"

#include "Runtime/Graphics/Public/Resource/IGPUResource.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class INSIGHT_API D3D12GPUResource : public IGPUResource
			{
			public:
				D3D12GPUResource(ID3D12Resource* pResource, EResourceState CurrentState);
				D3D12GPUResource();
				virtual ~D3D12GPUResource();

				virtual void Destroy() override;

				//
				// Getters/Setters
				//
				ID3D12Resource* GetResource() const { return m_pID3D12Resource.Get(); }
				D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return m_GpuVirtualAddress; }

				ID3D12Resource* operator -> () { return m_pID3D12Resource.Get(); }
				const ID3D12Resource* operator->() const { return m_pID3D12Resource.Get(); }
				ID3D12Resource** GetAddressOf() { return m_pID3D12Resource.GetAddressOf(); }

			protected:
				Microsoft::WRL::ComPtr<ID3D12Resource> m_pID3D12Resource;
				D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;
			};
		}
	}
}