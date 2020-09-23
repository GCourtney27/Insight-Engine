#pragma once

#include <Insight/Core.h>
#include "Platform/Windows/Error/COM_Exception.h"
#include <d3d12.h>
#include <wrl/client.h>

namespace Insight {

	using Microsoft::WRL::ComPtr;

	template <typename ConstantBufferType>
	class ieD3D12ConstantBuffer
	{
	public:
		ieD3D12ConstantBuffer() = default;
		~ieD3D12ConstantBuffer() = default;
		ieD3D12ConstantBuffer(ieD3D12ConstantBuffer& rhs) = delete;

		ConstantBufferType Data = {};

		bool Init(ID3D12Device* pDevice, LPCWSTR DebugName)
		{
			HRESULT hr = 
			pDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_pResource));
			m_pResource->SetName(DebugName);
			
			ThrowIfFailed(hr, "Failed to create upload heap for %s upload resource heap.");
			CD3DX12_RANGE ReadRange(0, 0);
			hr = m_pResource->Map(0, &ReadRange, reinterpret_cast<void**>(&m_GPUAddress));
			ThrowIfFailed(hr, "Failed to create map heap for per-frame upload resource heaps");
			m_pResource->Unmap(0, nullptr);

			return true;
		}

		void SubmitToGPU()
		{
			uint8_t* pIntermediateAddress;
			m_pResource->Map(0, nullptr, (void**)&pIntermediateAddress);
			memcpy(m_GPUAddress, &Data, sizeof(ConstantBufferType));
			m_pResource->Unmap(0, nullptr);
		}
		
		ID3D12Resource* GetResource() const
		{
			return m_pResource.Get();
		}

		UINT8* GetGPUAddress() const
		{
			return m_GPUAddress;
		}

		inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const
		{
			return m_pResource->GetGPUVirtualAddress();
		}

		inline int GetBufferSize() const
		{
			return m_BufferSize;
		}

		void SetAsGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex)
		{
			pCommandList->SetGraphicsRootConstantBufferView(RootParameterIndex, GetGPUVirtualAddress());
		}

		void SetAsComputeRootConstantBufferView(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex)
		{
			pCommandList->SetComputeRootConstantBufferView(RootParameterIndex, GetGPUVirtualAddress());
		}

	private:
		int m_BufferSize = (sizeof(ConstantBufferType) + 255) & ~255;
		ComPtr<ID3D12Resource> m_pResource;
		UINT8* m_GPUAddress = nullptr;
	};

}
