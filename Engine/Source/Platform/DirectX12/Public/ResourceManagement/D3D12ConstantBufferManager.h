#pragma once

#include <Runtime/Core.h>
#include <Runtime/Graphics/Public/GraphicsCore.h>

#include "Runtime/Graphics/Public/ResourceManagement/IConstantBufferManager.h"
#include "Platform/DirectX12/Public/Resource/D3D12GPUResource.h"

#include "Platform/DirectX12/Private/D3D12BackendCore.h"
#include "Runtime/Graphics/Public/IDevice.h"
#include "Platform/Public/Utility/COMException.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class INSIGHT_API D3D12ConstantBuffer : public IConstantBuffer, public D3D12GPUResource
			{
				friend class D3D12ConstantBufferManager;
				friend class D3D12CommandContext;
			public:
				D3D12ConstantBuffer()
					: m_CBV()
					, m_pWritePointer(NULL)
				{
				}

				FORCE_INLINE D3D12_CPU_DESCRIPTOR_HANDLE CBV() const { return m_CBV; }
				FORCE_INLINE void* GetGPUWritePointer() const { return m_pWritePointer; }


			protected:
				void UploadBuffer()
				{
					memcpy(GetGPUWritePointer(), RCast<const void*>(m_Data), GetBufferSize());
				}

				virtual void Create(const EString& Name, UInt32 BufferSize) override
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

				FORCE_INLINE UInt32 GetAlignedBufferSize(UInt32 BufferSize)
				{
					return (BufferSize + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1); // Must be a multiple 256 bytes
				}

			private:
				D3D12_CPU_DESCRIPTOR_HANDLE m_CBV;
				void* m_pWritePointer;
			};




			class INSIGHT_API D3D12ConstantBufferManager : public IConstantBufferManager
			{
			public:
				D3D12ConstantBufferManager()
					: m_pID3D12DeviceRef(NULL)
				{
					m_pID3D12DeviceRef = RCast<ID3D12Device*>(g_pDevice->GetNativeDevice());
					IE_ASSERT(m_pID3D12DeviceRef != NULL);

				}
				virtual ~D3D12ConstantBufferManager() 
				{
					m_pID3D12DeviceRef = NULL;
				}

				virtual void CreateConstantBuffer(const EString& Name, IConstantBuffer** OutBuffer, UInt32 BufferSizeInBytes) override
				{
					ConstantBufferUID NewID = s_NextAvailableBufferID++;

					auto InsertResult = m_ConstantBufferLUT.try_emplace(NewID, D3D12ConstantBuffer{});
					IE_ASSERT(InsertResult.second == true);

					D3D12ConstantBuffer& ConstBuffer = m_ConstantBufferLUT.at(NewID);
					ConstBuffer.SetUID(NewID);
					ConstBuffer.Create(Name, BufferSizeInBytes);

					(*OutBuffer) = &ConstBuffer;
				}

			private:
				ID3D12Device* m_pID3D12DeviceRef;
				std::unordered_map<ConstantBufferUID, D3D12ConstantBuffer> m_ConstantBufferLUT;
				
			};
		}
	}
}
