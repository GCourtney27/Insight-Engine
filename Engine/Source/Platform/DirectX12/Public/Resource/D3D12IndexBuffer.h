#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/Resource/IIndexBuffer.h"
#include "Platform/DirectX12/Public/Resource/D3D12GPUResource.h"

#include "Platform/Public/Utility/COMException.h" // TEMP!!
#include "Runtime/Graphics/Public/IDevice.h"
#include "Runtime/Graphics/Public/ICommandContext.h"

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
					IE_ASSERT(pID3D12Device != NULL)
					auto ResDesc = CD3DX12_RESOURCE_DESC::Buffer(IndexDataSize);


					// Initialize GPU Resource.
					//
					ThrowIfFailed(pID3D12Device->CreateCommittedResource(
						&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
						D3D12_HEAP_FLAG_NONE,
						&ResDesc,
						D3D12_RESOURCE_STATE_COPY_DEST,
						nullptr,
						IID_PPV_ARGS(&m_pID3D12Resource)), TEXT("Failed to create commited resource!"));
#if IE_DEBUG
					m_pID3D12Resource->SetName(Name.c_str());
#endif // IE_DEBUG


					// Initialize Upload Heap.
					//
					D3D12GPUResource UploadHeap;
					ThrowIfFailed(pID3D12Device->CreateCommittedResource(
						&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
						D3D12_HEAP_FLAG_NONE,
						&ResDesc,
						D3D12_RESOURCE_STATE_GENERIC_READ,
						nullptr,
						IID_PPV_ARGS(UploadHeap.GetAddressOf())
					), TEXT("Failed to create upload heap for vertex buffer!"));

					SubResourceData vertexData = {};
					vertexData.pData = pIndices;
					vertexData.RowPitch = IndexDataSize;
					vertexData.SlicePitch = IndexDataSize;

					ICommandContext& InitContext = ICommandContext::Begin(L"");
					{
						InitContext.UpdateSubresources(*this, UploadHeap, 0, 0, 1, vertexData);
					}
					InitContext.Finish(true);
					//UploadHeap.DestroyCOMResource();

					// Initialize the Index Buffer View.
					//
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