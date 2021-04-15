#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/Resource/IVertexBuffer.h"
#include "Platform/DirectX12/Public/Resource/D3D12GPUResource.h"

#include "Platform/Public/Utility/COMException.h" // TEMP!!

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class INSIGHT_API D3D12VertexBuffer : public IVertexBuffer, public D3D12GPUResource
			{
			public:
				D3D12VertexBuffer() {}
				~D3D12VertexBuffer() {}

				void Create(ID3D12Device* pDevice, UInt32 BufferSize)
				{
					auto HeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
					auto ResDesc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);
					ThrowIfFailed(pDevice->CreateCommittedResource(
						&HeapProps,
						D3D12_HEAP_FLAG_NONE,
						&ResDesc,
						D3D12_RESOURCE_STATE_GENERIC_READ,
						nullptr,
						IID_PPV_ARGS(&m_pID3D12Resource)), TEXT("Failed to create commited resource!"));
				}

				virtual void* GetNativeBufferView() override { return RCast<void*>(&m_D3D12VertexBufferView); }

			protected:
				D3D12_VERTEX_BUFFER_VIEW m_D3D12VertexBufferView;
			};
		}
	}
}