#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/Resource/IVertexBuffer.h"
#include "Platform/DirectX12/Public/Resource/D3D12GPUResource.h"

#include "Platform/Public/Utility/COMException.h" // TEMP!!
#include "Runtime/Graphics/Public/IDevice.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class INSIGHT_API D3D12VertexBuffer : public IVertexBuffer, public D3D12GPUResource
			{
				friend class D3D12GeometryManager;
			public:
				D3D12VertexBuffer() {}
				~D3D12VertexBuffer() {}

				virtual void* GetNativeBufferView() override { return RCast<void*>(&m_D3D12VertexBufferView); }

				virtual void Create(const EString& Name, UInt32 VertexDataSize, UInt32 VertexStrideSize, void* pVerticies) override
				{
					ID3D12Device* pID3D12Device = RCast<ID3D12Device*>(g_pDevice->GetNativeDevice());

					auto HeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
					auto ResDesc = CD3DX12_RESOURCE_DESC::Buffer(VertexDataSize);
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
					memcpy(pVertexDataBegin, pVerticies, (size_t)VertexDataSize);
					m_pID3D12Resource->Unmap(0, nullptr);

					// Initialize the vertex buffer view.
					m_D3D12VertexBufferView.BufferLocation = m_pID3D12Resource->GetGPUVirtualAddress();
					m_D3D12VertexBufferView.StrideInBytes = VertexStrideSize;
					m_D3D12VertexBufferView.SizeInBytes = VertexDataSize;

#if IE_DEBUG
					m_pID3D12Resource->SetName(Name.c_str());
#endif // IE_DEBUG
				}

			protected:
				D3D12_VERTEX_BUFFER_VIEW m_D3D12VertexBufferView;
			};
		}
	}
}