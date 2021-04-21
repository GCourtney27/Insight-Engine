#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/Resource/IIndexBuffer.h"
#include "Platform/DirectX12/Public/Resource/D3D12GPUResource.h"

#include "Platform/Public/Utility/COMException.h" // TEMP!!
#include "Runtime/Graphics/Public/IDevice.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class INSIGHT_API D3D12IndexBuffer : public IIndexBuffer, public D3D12GPUResource
			{
				friend class D3D12GeometryBufferManager;
			public:
				D3D12IndexBuffer() {}
				~D3D12IndexBuffer() {}
				
				virtual void* GetNativeBufferView() override { return RCast<void*>(&m_D3D12IndexBufferView); }

				virtual void Create(const EString& Name, UInt32 IndexDataSize, void* pIndices) override
				{
					ID3D12Device* pID3D12Device = RCast<ID3D12Device*>(g_pDevice->GetNativeDevice());

					auto HeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
					auto ResDesc = CD3DX12_RESOURCE_DESC::Buffer(IndexDataSize);
					ThrowIfFailed(pID3D12Device->CreateCommittedResource(
						&HeapProps,
						D3D12_HEAP_FLAG_NONE,
						&ResDesc,
						D3D12_RESOURCE_STATE_GENERIC_READ,
						nullptr,
						IID_PPV_ARGS(&m_pID3D12Resource)), TEXT("Failed to create commited resource!"));

					// Copy the triangle data to the vertex buffer.
					UINT8* pVertexDataBegin;
					CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
					ThrowIfFailed(m_pID3D12Resource->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)), TEXT("Failed to map memory!"));
					memcpy(pVertexDataBegin, pIndices, (size_t)IndexDataSize);
					m_pID3D12Resource->Unmap(0, nullptr);

					m_D3D12IndexBufferView.BufferLocation = m_pID3D12Resource->GetGPUVirtualAddress();
					m_D3D12IndexBufferView.Format = (DXGI_FORMAT)F_R32_UINT;
					m_D3D12IndexBufferView.SizeInBytes = IndexDataSize;
				}


			protected:
				D3D12_INDEX_BUFFER_VIEW m_D3D12IndexBufferView;

			};
		}
	}
}