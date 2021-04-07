#include <Engine_pch.h>

#include "Platform/DirectX12/Public/Common/D3D12Utility.h"
#include "Platform/DirectX12/Public/Resource/D3D12GPUResource.h"
#include "..\..\Public\Resource\D3D12GPUResource.h"


namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			D3D12GPUResource::D3D12GPUResource(ID3D12Resource* pResource, EResourceState CurrentState)
				: IGPUResource(CurrentState, RESOURCE_STATE_INVALID)
				, m_pID3D12Resource(pResource)
				, m_GpuVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS_NULL)
			{
			}

			D3D12GPUResource::D3D12GPUResource()
				: m_pID3D12Resource(NULL)
				, m_GpuVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS_NULL)
			{
			}

			D3D12GPUResource::~D3D12GPUResource()
			{
				Destroy();
			}

			void D3D12GPUResource::Destroy()
			{
				m_pID3D12Resource = nullptr;
				m_GpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
				++m_VersionID;
			}
		}
	}
}
