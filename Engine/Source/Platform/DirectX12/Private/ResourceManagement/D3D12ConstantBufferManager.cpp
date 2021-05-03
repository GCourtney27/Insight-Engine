#include <Engine_pch.h>

#include "Platform/DirectX12/Public/ResourceManagement/D3D12ConstantBufferManager.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			// -------------------
			// D3D12ConstantBuffer
			// -------------------

			void D3D12ConstantBuffer::Create(const EString& Name, UInt32 BufferSize)
			{
				SetBufferSize(GetAlignedBufferSize(BufferSize));

				ID3D12Device* pID3D12Device = RCast<ID3D12Device*>(g_pDevice->GetNativeDevice());
				auto ResDesc = CD3DX12_RESOURCE_DESC::Buffer(GetBufferSize());

				ThrowIfFailed(pID3D12Device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE,
					&ResDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_pID3D12Resource)
				), TEXT("Failed to create default heap for vertex buffer!"));
#if IE_DEBUG
				m_pID3D12Resource->SetName(Name.c_str());
#endif // IE_DEBUG
				m_GpuVirtualAddress = m_pID3D12Resource->GetGPUVirtualAddress();

				CD3DX12_RANGE ReadRange(0, 0);        // We do not intend to read from this resource on the CPU.
				ThrowIfFailed(m_pID3D12Resource->Map(0, &ReadRange, &m_pWritePointer), TEXT("Failed to create committed resource for constant buffer."));

				m_CBV = AllocateDescriptor(pID3D12Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc = {};
				CBVDesc.SizeInBytes = GetBufferSize();
				CBVDesc.BufferLocation = m_pID3D12Resource->GetGPUVirtualAddress();
				pID3D12Device->CreateConstantBufferView(&CBVDesc, m_CBV);
			}


			// ---------------------------
			// D3D12ConstantBufferManager
			// ---------------------------

			void D3D12ConstantBufferManager::CreateConstantBuffer(const EString& Name, IConstantBuffer** OutBuffer, UInt32 BufferSizeInBytes)
			{
				ConstantBufferUID NewID = s_NextAvailableBufferID++;

				auto InsertResult = m_ConstantBufferLUT.try_emplace(NewID, D3D12ConstantBuffer{});
				IE_ASSERT(InsertResult.second == true);

				D3D12ConstantBuffer& ConstBuffer = m_ConstantBufferLUT.at(NewID);
				ConstBuffer.SetUID(NewID);
				ConstBuffer.Create(Name, BufferSizeInBytes);

				(*OutBuffer) = &ConstBuffer;
			}


			void D3D12ConstantBufferManager::DestroyConstantBuffer(ConstantBufferUID BufferHandle)
			{
				IE_ASSERT(BufferHandle != IE_INVALID_CONSTANT_BUFFER_HANDLE); // Trying to destroy a constant buffer with an invalid handle.

				auto Iter = m_ConstantBufferLUT.find(BufferHandle);
				if (Iter != m_ConstantBufferLUT.end())
				{
					m_ConstantBufferLUT.erase(Iter);
				}
			}
		}
	}
}
