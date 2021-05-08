#pragma once

#include <Runtime/Core.h>
#include <Runtime/Graphics/Public/GraphicsCore.h>

#include "Platform/DirectX12/Public/Resource/D3D12GPUResource.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{

			// Constant blocks must be multiples of 16 constants @ 16 bytes each
#define DEFAULT_ALIGN 256

			// Various types of allocations may contain NULL pointers.  Check before dereferencing if you are unsure.
			struct DynAlloc
			{
				DynAlloc(D3D12GPUResource& BaseResource, UInt64 ThisOffset, UInt64 ThisSize)
					: Buffer(BaseResource)
					, Offset(ThisOffset)
					, Size(ThisSize)
				{
				}

				D3D12GPUResource& Buffer;	// The D3D buffer associated with this memory.
				UInt64 Offset;			// Offset from start of buffer resource
				UInt64 Size;			// Reserved size of this allocation
				void* DataPtr;			// The CPU-writeable address
				D3D12_GPU_VIRTUAL_ADDRESS GpuAddress;	// The GPU-visible address
			};


            class LinearAllocationPage : public D3D12GPUResource
            {
            public:
                LinearAllocationPage(ID3D12Resource* pResource, EResourceState Usage) : D3D12GPUResource()
                {
                    m_pID3D12Resource.Attach(pResource);
                    m_UsageState = Usage;
                    m_GpuVirtualAddress = m_pID3D12Resource->GetGPUVirtualAddress();
                    m_pID3D12Resource->Map(0, nullptr, &m_CpuVirtualAddress);
                }

                ~LinearAllocationPage()
                {
                    Unmap();
                }

                void Map(void)
                {
                    if (m_CpuVirtualAddress == nullptr)
                    {
                        m_pID3D12Resource->Map(0, nullptr, &m_CpuVirtualAddress);
                    }
                }

                void Unmap(void)
                {
                    if (m_CpuVirtualAddress != nullptr)
                    {
                        m_pID3D12Resource->Unmap(0, nullptr);
                        m_CpuVirtualAddress = nullptr;
                    }
                }

                void* m_CpuVirtualAddress;
                D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;
            };

            enum LinearAllocatorType
            {
                LAT_InvalidAllocator = -1,

                LAT_GpuExclusive = 0,		// DEFAULT   GPU-writeable (via UAV)
                LAT_CpuWritable = 1,		// UPLOAD CPU-writeable (but write combined)

                LAT_NumAllocatorTypes
            };

            enum
            {
                GpuAllocatorPageSize = 0x10000,	// 64K
                CpuAllocatorPageSize = 0x200000	// 2MB
            };

            class LinearAllocatorPageManager
            {
            public:

                LinearAllocatorPageManager();
                LinearAllocationPage* RequestPage(void);
                LinearAllocationPage* CreateNewPage(size_t PageSize = 0);

                // Discarded pages will get recycled.  This is for fixed size pages.
                void DiscardPages(uint64_t FenceID, const std::vector<LinearAllocationPage*>& Pages);

                // Freed pages will be destroyed once their fence has passed.  This is for single-use,
                // "large" pages.
                void FreeLargePages(uint64_t FenceID, const std::vector<LinearAllocationPage*>& Pages);

                void Destroy(void) { m_PagePool.clear(); }

            private:

                static LinearAllocatorType sm_AutoType;

                LinearAllocatorType m_AllocationType;
                std::vector<std::unique_ptr<LinearAllocationPage> > m_PagePool;
                std::queue<std::pair<uint64_t, LinearAllocationPage*> > m_RetiredPages;
                std::queue<std::pair<uint64_t, LinearAllocationPage*> > m_DeletionQueue;
                std::queue<LinearAllocationPage*> m_AvailablePages;
                std::mutex m_Mutex;
            };

            class LinearAllocator
            {
            public:

                LinearAllocator(LinearAllocatorType Type) : m_AllocationType(Type), m_PageSize(0), m_CurOffset(~(size_t)0), m_CurPage(nullptr)
                {
                    IE_ASSERT(Type > LAT_InvalidAllocator && Type < LAT_NumAllocatorTypes);
                    m_PageSize = (Type == LAT_GpuExclusive ? GpuAllocatorPageSize : CpuAllocatorPageSize);
                }

                DynAlloc Allocate(size_t SizeInBytes, size_t Alignment = DEFAULT_ALIGN);

                void CleanupUsedPages(uint64_t FenceID);

                static void DestroyAll(void)
                {
                    sm_PageManager[0].Destroy();
                    sm_PageManager[1].Destroy();
                }

            private:

                DynAlloc AllocateLargePage(size_t SizeInBytes);

                static LinearAllocatorPageManager sm_PageManager[2];

                LinearAllocatorType m_AllocationType;
                size_t m_PageSize;
                size_t m_CurOffset;
                LinearAllocationPage* m_CurPage;
                std::vector<LinearAllocationPage*> m_RetiredPages;
                std::vector<LinearAllocationPage*> m_LargePageList;
            };
		}
	}
}
