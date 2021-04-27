#pragma once

#include <Runtime/Core.h>
#include <Runtime/Graphics/Public/GraphicsCore.h>

#include "Platform/DirectX12/Public/Common/D3D12Utility.h"


namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class INSIGHT_API D3D12DescriptorAllocator
			{
			public:
				D3D12DescriptorAllocator(EResourceHeapType Type) 
					: m_Type((D3D12_DESCRIPTOR_HEAP_TYPE)Type)
					, m_CurrentHeap(NULL)
					, m_DescriptorSize(0)
				{
					m_CurrentHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
				}
				virtual ~D3D12DescriptorAllocator() {}
				
				D3D12_CPU_DESCRIPTOR_HANDLE Allocate(ID3D12Device* pDevice, UInt32 Count);
				static void DestroyAll(void);

			protected:

				static const uint32_t sm_NumDescriptorsPerHeap = 256;
				static std::mutex sm_AllocationMutex;
				static std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> sm_DescriptorHeapPool;
				static ID3D12DescriptorHeap* RequestNewHeap(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE Type);

				D3D12_DESCRIPTOR_HEAP_TYPE m_Type;
				ID3D12DescriptorHeap* m_CurrentHeap;
				D3D12_CPU_DESCRIPTOR_HANDLE m_CurrentHandle;
				uint32_t m_DescriptorSize;
				uint32_t m_RemainingFreeHandles;
			};
		}
	}
}
