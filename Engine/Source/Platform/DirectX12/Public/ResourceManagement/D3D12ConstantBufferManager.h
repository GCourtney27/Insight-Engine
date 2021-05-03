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
				~D3D12ConstantBuffer() = default;

				FORCE_INLINE D3D12_CPU_DESCRIPTOR_HANDLE CBV() const;
				FORCE_INLINE void* GetGPUWritePointer() const;

			protected:
				FORCE_INLINE void UploadBuffer();
				FORCE_INLINE static UInt32 GetAlignedBufferSize(UInt32 BufferSize);

				virtual void Create(const EString& Name, UInt32 BufferSize) override;

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

				virtual void CreateConstantBuffer(const EString& Name, IConstantBuffer** OutBuffer, UInt32 BufferSizeInBytes) override;
				virtual void DestroyConstantBuffer(ConstantBufferUID BufferHandle);

			private:
				ID3D12Device* m_pID3D12DeviceRef;
				std::unordered_map<ConstantBufferUID, D3D12ConstantBuffer> m_ConstantBufferLUT;
			};

			//
			// Inline function implementations
			//

			// D3D12ConstantBuffer

			FORCE_INLINE D3D12_CPU_DESCRIPTOR_HANDLE D3D12ConstantBuffer::CBV() const
			{ 
				return m_CBV; 
			}
			
			FORCE_INLINE void* D3D12ConstantBuffer::GetGPUWritePointer() const
			{ 
				return m_pWritePointer; 
			}

			FORCE_INLINE void D3D12ConstantBuffer::UploadBuffer()
			{
				memcpy(GetGPUWritePointer(), RCast<const void*>(m_Data), GetBufferSize());
			}

			FORCE_INLINE UInt32 D3D12ConstantBuffer::GetAlignedBufferSize(UInt32 BufferSize)
			{
				return (BufferSize + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1); // Must be a multiple 256 bytes
			}
		}
	}
}
