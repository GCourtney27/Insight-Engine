#pragma once

#include <Insight/Core.h>
#include "Platform/Windows/Error/COM_Exception.h"
#include <d3d12.h>
#include <wrl/client.h>

namespace Insight {

	using Microsoft::WRL::ComPtr;

	template <typename ConstantBufferType>
	class CD3D12ConstantBuffer
	{
	public:
		CD3D12ConstantBuffer() = default;
		~CD3D12ConstantBuffer() = default;
		CD3D12ConstantBuffer(CD3D12ConstantBuffer& rhs) = delete;

		ConstantBufferType Data = {};

		bool Init(ID3D12Device* pDevice, LPCWSTR DebugName)
		{
			HRESULT hr = 
			pDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(m_BufferSize),
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
	
		inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const
		{
			return m_pResource->GetGPUVirtualAddress();
		}

		inline int GetBufferSize() const
		{
			return m_BufferSize;
		}

		void SetAsRootConstantBufferView(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex)
		{
			pCommandList->SetGraphicsRootConstantBufferView(RootParameterIndex, GetGPUVirtualAddress());
		}

	private:
		int m_BufferSize = (sizeof(ConstantBufferType) + 255) & ~255;
		ComPtr<ID3D12Resource> m_pResource;
		UINT8* m_GPUAddress = nullptr;
	};

}
