#pragma once

#include <Runtime/Core.h>

#include "Platform/DirectX12/Public/Common/D3D12Utility.h"

#include "Platform/DirectX12/Public/D3D12DescriptorAllocator.h"


namespace Insight
{
    namespace Graphics
    {
        namespace DX12
        {
            class D3D12CommandContext;

            class INSIGHT_API D3D12DynamicDescriptorHeap
            {
            public:
                D3D12DynamicDescriptorHeap(D3D12CommandContext& OwningContext, D3D12_DESCRIPTOR_HEAP_TYPE HeapType);
                ~D3D12DynamicDescriptorHeap();

                static void DestroyAll(void)
                {
                    sm_DescriptorHeapPool[0].clear();
                    sm_DescriptorHeapPool[1].clear();
                }

                void CleanupUsedHeaps(UInt64 fenceValue);

                // Copy multiple handles into the cache area reserved for the specified root parameter.
                void SetGraphicsDescriptorHandles(UInt32 RootIndex, UInt32 Offset, UInt32 NumHandles, const D3D12_CPU_DESCRIPTOR_HANDLE Handles[])
                {
                    m_GraphicsHandleCache.StageDescriptorHandles(RootIndex, Offset, NumHandles, Handles);
                }

                void SetComputeDescriptorHandles(UInt32 RootIndex, UInt32 Offset, UInt32 NumHandles, const D3D12_CPU_DESCRIPTOR_HANDLE Handles[])
                {
                    m_ComputeHandleCache.StageDescriptorHandles(RootIndex, Offset, NumHandles, Handles);
                }

                // Bypass the cache and upload directly to the shader-visible heap
                D3D12_GPU_DESCRIPTOR_HANDLE UploadDirect(D3D12_CPU_DESCRIPTOR_HANDLE Handles);

                // TODO
                // Deduce cache layout needed to support the descriptor tables needed by the root signature.
                /*void ParseGraphicsRootSignature(const RootSignature& RootSig)
                {
                    m_GraphicsHandleCache.ParseRootSignature(m_DescriptorType, RootSig);
                }

                void ParseComputeRootSignature(const RootSignature& RootSig)
                {
                    m_ComputeHandleCache.ParseRootSignature(m_DescriptorType, RootSig);
                }*/

                // Upload any new descriptors in the cache to the shader-visible heap.
                inline void CommitGraphicsRootDescriptorTables(ID3D12GraphicsCommandList* CmdList)
                {
                    if (m_GraphicsHandleCache.m_StaleRootParamsBitMap != 0)
                        CopyAndBindStagedTables(m_GraphicsHandleCache, CmdList, &ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable);
                }

                inline void CommitComputeRootDescriptorTables(ID3D12GraphicsCommandList* CmdList)
                {
                    if (m_ComputeHandleCache.m_StaleRootParamsBitMap != 0)
                        CopyAndBindStagedTables(m_ComputeHandleCache, CmdList, &ID3D12GraphicsCommandList::SetComputeRootDescriptorTable);
                }

            private:

                // Static members
                static const uint32_t kNumDescriptorsPerHeap = 1024;
                static std::mutex sm_Mutex;
                static std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> sm_DescriptorHeapPool[2];
                static std::queue<std::pair<uint64_t, ID3D12DescriptorHeap*>> sm_RetiredDescriptorHeaps[2];
                static std::queue<ID3D12DescriptorHeap*> sm_AvailableDescriptorHeaps[2];

                // Static methods
                static ID3D12DescriptorHeap* RequestDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE HeapType);
                static void DiscardDescriptorHeaps(D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint64_t FenceValueForReset, const std::vector<ID3D12DescriptorHeap*>& UsedHeaps);

                // Non-static members
                D3D12CommandContext& m_OwningContext;
                ID3D12DescriptorHeap* m_CurrentHeapPtr;
                const D3D12_DESCRIPTOR_HEAP_TYPE m_DescriptorType;
                uint32_t m_DescriptorSize;
                uint32_t m_CurrentOffset;
                DescriptorHandle m_FirstDescriptor;
                std::vector<ID3D12DescriptorHeap*> m_RetiredHeaps;

                // Describes a descriptor table entry:  a region of the handle cache and which handles have been set
                struct DescriptorTableCache
                {
                    DescriptorTableCache() : AssignedHandlesBitMap(0) {}
                    uint32_t AssignedHandlesBitMap;
                    D3D12_CPU_DESCRIPTOR_HANDLE* TableStart;
                    uint32_t TableSize;
                };

                struct DescriptorHandleCache
                {
                    DescriptorHandleCache()
                    {
                        ClearCache();
                    }

                    void ClearCache()
                    {
                        m_RootDescriptorTablesBitMap = 0;
                        m_MaxCachedDescriptors = 0;
                    }

                    UInt32 m_RootDescriptorTablesBitMap = 0;
                    UInt32 m_StaleRootParamsBitMap = 0;
                    UInt32 m_MaxCachedDescriptors = 0;

                    static const UInt32 kMaxNumDescriptors = 256;
                    static const UInt32 kMaxNumDescriptorTables = 16;

                    UInt32 ComputeStagedSize();
                    void CopyAndBindStaleTables(D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t DescriptorSize, DescriptorHandle DestHandleStart, ID3D12GraphicsCommandList* CmdList,
                        void (STDMETHODCALLTYPE ID3D12GraphicsCommandList::* SetFunc)(UINT, D3D12_GPU_DESCRIPTOR_HANDLE));

                    DescriptorTableCache m_RootDescriptorTable[kMaxNumDescriptorTables];
                    D3D12_CPU_DESCRIPTOR_HANDLE m_HandleCache[kMaxNumDescriptors];

                    void UnbindAllValid();
                    void StageDescriptorHandles(UInt32 RootIndex, UInt32 Offset, UInt32 NumHandles, const D3D12_CPU_DESCRIPTOR_HANDLE Handles[]);
                    //void ParseRootSignature(D3D12_DESCRIPTOR_HEAP_TYPE Type, const RootSignature& RootSig);
                };

                DescriptorHandleCache m_GraphicsHandleCache;
                DescriptorHandleCache m_ComputeHandleCache;

                bool HasSpace(uint32_t Count)
                {
                    return (m_CurrentHeapPtr != nullptr && m_CurrentOffset + Count <= kNumDescriptorsPerHeap);
                }

                void RetireCurrentHeap(void);
                void RetireUsedHeaps(uint64_t fenceValue);
                ID3D12DescriptorHeap* GetHeapPointer();

                DescriptorHandle Allocate(UInt32 Count)
                {
                    DescriptorHandle ret = m_FirstDescriptor + m_CurrentOffset * m_DescriptorSize;
                    m_CurrentOffset += Count;
                    return ret;
                }

                void CopyAndBindStagedTables(DescriptorHandleCache& HandleCache, ID3D12GraphicsCommandList* CmdList,
                    void (STDMETHODCALLTYPE ID3D12GraphicsCommandList::* SetFunc)(UINT, D3D12_GPU_DESCRIPTOR_HANDLE));

                // Mark all descriptors in the cache as stale and in need of re-uploading.
                void UnbindAllValid(void);
            };
        }
    }
}
