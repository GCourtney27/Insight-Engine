#include <Engine_pch.h>

#include "Platform/DirectX12/Private/D3D12DynamicDescriptorHeap.h"

#include "Platform/DirectX12/Private/D3D12CommandManager.h"
#include "Platform/DirectX12/Public/D3D12CommandContext.h"

#include "Runtime/Graphics/Public/IDevice.h"


namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{

            //
            // D3D12DynamicDescriptorHeap Implementation
            //

            std::mutex D3D12DynamicDescriptorHeap::sm_Mutex;
            std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> D3D12DynamicDescriptorHeap::sm_DescriptorHeapPool[2];
            std::queue<std::pair<uint64_t, ID3D12DescriptorHeap*>> D3D12DynamicDescriptorHeap::sm_RetiredDescriptorHeaps[2];
            std::queue<ID3D12DescriptorHeap*> D3D12DynamicDescriptorHeap::sm_AvailableDescriptorHeaps[2];

            ID3D12DescriptorHeap* D3D12DynamicDescriptorHeap::RequestDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE HeapType)
            {
                std::lock_guard<std::mutex> LockGuard(sm_Mutex);

                uint32_t idx = HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER ? 1 : 0;
                D3D12CommandManager* CmdManager = DCast<D3D12CommandManager*>(g_pCommandManager);
                
                while (!sm_RetiredDescriptorHeaps[idx].empty() && CmdManager->IsFenceComplete(sm_RetiredDescriptorHeaps[idx].front().first))
                {
                    sm_AvailableDescriptorHeaps[idx].push(sm_RetiredDescriptorHeaps[idx].front().second);
                    sm_RetiredDescriptorHeaps[idx].pop();
                }

                if (!sm_AvailableDescriptorHeaps[idx].empty())
                {
                    ID3D12DescriptorHeap* HeapPtr = sm_AvailableDescriptorHeaps[idx].front();
                    sm_AvailableDescriptorHeaps[idx].pop();
                    return HeapPtr;
                }
                else
                {
                    ID3D12Device* pID3D12Device = RCast<ID3D12Device*>(g_pDevice->GetNativeDevice());
                    IE_ASSERT(pID3D12Device != NULL);

                    D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
                    HeapDesc.Type = HeapType;
                    HeapDesc.NumDescriptors = kNumDescriptorsPerHeap;
                    HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                    HeapDesc.NodeMask = 1;
                    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> HeapPtr;
                    pID3D12Device->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&HeapPtr));
                    sm_DescriptorHeapPool[idx].emplace_back(HeapPtr);
                    return HeapPtr.Get();
                }
            }

            void D3D12DynamicDescriptorHeap::DiscardDescriptorHeaps(D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint64_t FenceValue, const std::vector<ID3D12DescriptorHeap*>& UsedHeaps)
            {
                uint32_t idx = HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER ? 1 : 0;
                std::lock_guard<std::mutex> LockGuard(sm_Mutex);
                for (auto iter = UsedHeaps.begin(); iter != UsedHeaps.end(); ++iter)
                    sm_RetiredDescriptorHeaps[idx].push(std::make_pair(FenceValue, *iter));
            }

            void D3D12DynamicDescriptorHeap::RetireCurrentHeap(void)
            {
                // Don't retire unused heaps.
                if (m_CurrentOffset == 0)
                {
                    IE_ASSERT(m_CurrentHeapPtr == nullptr);
                    return;
                }

                IE_ASSERT(m_CurrentHeapPtr != nullptr);
                m_RetiredHeaps.push_back(m_CurrentHeapPtr);
                m_CurrentHeapPtr = nullptr;
                m_CurrentOffset = 0;
            }

            void D3D12DynamicDescriptorHeap::RetireUsedHeaps(uint64_t fenceValue)
            {
                DiscardDescriptorHeaps(m_DescriptorType, fenceValue, m_RetiredHeaps);
                m_RetiredHeaps.clear();
            }

            D3D12DynamicDescriptorHeap::D3D12DynamicDescriptorHeap(D3D12CommandContext& OwningContext, D3D12_DESCRIPTOR_HEAP_TYPE HeapType)
                : m_OwningContext(OwningContext)
                , m_DescriptorType(HeapType)
            {
                m_CurrentHeapPtr = nullptr;
                m_CurrentOffset = 0;

                ID3D12Device* pID3D12Device = RCast<ID3D12Device*>(g_pDevice->GetNativeDevice());
                IE_ASSERT(pID3D12Device != NULL);

                m_DescriptorSize = pID3D12Device->GetDescriptorHandleIncrementSize(HeapType);
            }

            D3D12DynamicDescriptorHeap::~D3D12DynamicDescriptorHeap()
            {
            }

            void D3D12DynamicDescriptorHeap::CleanupUsedHeaps(uint64_t fenceValue)
            {
                RetireCurrentHeap();
                RetireUsedHeaps(fenceValue);
                m_GraphicsHandleCache.ClearCache();
                m_ComputeHandleCache.ClearCache();
            }

            inline ID3D12DescriptorHeap* D3D12DynamicDescriptorHeap::GetHeapPointer()
            {
                if (m_CurrentHeapPtr == nullptr)
                {
                    IE_ASSERT(m_CurrentOffset == 0);
                    m_CurrentHeapPtr = RequestDescriptorHeap(m_DescriptorType);
                    CpuDescriptorHandle CpuHandle{ m_CurrentHeapPtr->GetCPUDescriptorHandleForHeapStart().ptr };
                    GpuDescriptorHandle GpuHandle{ m_CurrentHeapPtr->GetGPUDescriptorHandleForHeapStart().ptr };
                    m_CurrentHeapPtr->GetGPUDescriptorHandleForHeapStart();
                    m_FirstDescriptor = DescriptorHandle(CpuHandle, GpuHandle);
                }

                return m_CurrentHeapPtr;
            }

            UInt32 D3D12DynamicDescriptorHeap::DescriptorHandleCache::ComputeStagedSize()
            {
                // Sum the maximum assigned offsets of stale descriptor tables to determine total needed space.
                UInt32 NeededSpace = 0;
                UInt32 RootIndex;
                UInt32 StaleParams = m_StaleRootParamsBitMap;
                while (_BitScanForward((unsigned long*)&RootIndex, StaleParams))
                {
                    StaleParams ^= (1 << RootIndex);

                    UInt32 MaxSetHandle;
                    IE_ASSERT(TRUE == _BitScanReverse((unsigned long*)&MaxSetHandle, m_RootDescriptorTable[RootIndex].AssignedHandlesBitMap),
                        "Root entry marked as stale but has no stale descriptors");

                    NeededSpace += MaxSetHandle + 1;
                }
                return NeededSpace;
            }

            void D3D12DynamicDescriptorHeap::DescriptorHandleCache::CopyAndBindStaleTables(
                D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t DescriptorSize,
                DescriptorHandle DestHandleStart, ID3D12GraphicsCommandList* CmdList,
                void (STDMETHODCALLTYPE ID3D12GraphicsCommandList::* SetFunc)(UINT, D3D12_GPU_DESCRIPTOR_HANDLE))
            {
                uint32_t StaleParamCount = 0;
                uint32_t TableSize[DescriptorHandleCache::kMaxNumDescriptorTables];
                uint32_t RootIndices[DescriptorHandleCache::kMaxNumDescriptorTables];
                uint32_t NeededSpace = 0;
                uint32_t RootIndex;

                // Sum the maximum assigned offsets of stale descriptor tables to determine total needed space.
                uint32_t StaleParams = m_StaleRootParamsBitMap;
                while (_BitScanForward((unsigned long*)&RootIndex, StaleParams))
                {
                    RootIndices[StaleParamCount] = RootIndex;
                    StaleParams ^= (1 << RootIndex);

                    uint32_t MaxSetHandle;
                    IE_ASSERT(TRUE == _BitScanReverse((unsigned long*)&MaxSetHandle, m_RootDescriptorTable[RootIndex].AssignedHandlesBitMap),
                        "Root entry marked as stale but has no stale descriptors");

                    NeededSpace += MaxSetHandle + 1;
                    TableSize[StaleParamCount] = MaxSetHandle + 1;

                    ++StaleParamCount;
                }

                IE_ASSERT(StaleParamCount <= DescriptorHandleCache::kMaxNumDescriptorTables,
                    "We're only equipped to handle so many descriptor tables");

                m_StaleRootParamsBitMap = 0;

                static const uint32_t kMaxDescriptorsPerCopy = 16;
                UInt32 NumDestDescriptorRanges = 0;
                D3D12_CPU_DESCRIPTOR_HANDLE pDestDescriptorRangeStarts[kMaxDescriptorsPerCopy];
                UINT pDestDescriptorRangeSizes[kMaxDescriptorsPerCopy];

                UInt32 NumSrcDescriptorRanges = 0;
                D3D12_CPU_DESCRIPTOR_HANDLE pSrcDescriptorRangeStarts[kMaxDescriptorsPerCopy];
                UINT pSrcDescriptorRangeSizes[kMaxDescriptorsPerCopy];

                ID3D12Device* pID3D12Device = RCast<ID3D12Device*>(g_pDevice->GetNativeDevice());
                IE_ASSERT(pID3D12Device != NULL);


                for (uint32_t i = 0; i < StaleParamCount; ++i)
                {
                    RootIndex = RootIndices[i];
                    D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle{ DestHandleStart.GetGpuPtr() };
                    (CmdList->*SetFunc)(RootIndex, GpuHandle);

                    DescriptorTableCache& RootDescTable = m_RootDescriptorTable[RootIndex];

                    D3D12_CPU_DESCRIPTOR_HANDLE* SrcHandles = RootDescTable.TableStart;
                    uint64_t SetHandles = (uint64_t)RootDescTable.AssignedHandlesBitMap;
                    D3D12_CPU_DESCRIPTOR_HANDLE CurDest{ DestHandleStart.GetCpuPtr() };
                    DestHandleStart += TableSize[i] * DescriptorSize;

                    unsigned long SkipCount;
                    while (_BitScanForward64(&SkipCount, SetHandles))
                    {
                        // Skip over unset descriptor handles
                        SetHandles >>= SkipCount;
                        SrcHandles += SkipCount;
                        CurDest.ptr += SkipCount * DescriptorSize;

                        unsigned long DescriptorCount;
                        _BitScanForward64(&DescriptorCount, ~SetHandles);
                        SetHandles >>= DescriptorCount;

                        // If we run out of temp room, copy what we've got so far
                        if (NumSrcDescriptorRanges + DescriptorCount > kMaxDescriptorsPerCopy)
                        {
                            pID3D12Device->CopyDescriptors(
                                NumDestDescriptorRanges, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes,
                                NumSrcDescriptorRanges, pSrcDescriptorRangeStarts, pSrcDescriptorRangeSizes,
                                Type);

                            NumSrcDescriptorRanges = 0;
                            NumDestDescriptorRanges = 0;
                        }

                        // Setup destination range
                        pDestDescriptorRangeStarts[NumDestDescriptorRanges] = CurDest;
                        pDestDescriptorRangeSizes[NumDestDescriptorRanges] = DescriptorCount;
                        ++NumDestDescriptorRanges;

                        // Setup source ranges (one descriptor each because we don't assume they are contiguous)
                        for (uint32_t j = 0; j < DescriptorCount; ++j)
                        {
                            pSrcDescriptorRangeStarts[NumSrcDescriptorRanges] = SrcHandles[j];
                            pSrcDescriptorRangeSizes[NumSrcDescriptorRanges] = 1;
                            ++NumSrcDescriptorRanges;
                        }

                        // Move the destination pointer forward by the number of descriptors we will copy
                        SrcHandles += DescriptorCount;
                        CurDest.ptr += DescriptorCount * DescriptorSize;
                    }
                }

                pID3D12Device->CopyDescriptors(
                    NumDestDescriptorRanges, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes,
                    NumSrcDescriptorRanges, pSrcDescriptorRangeStarts, pSrcDescriptorRangeSizes,
                    Type);
            }

            void D3D12DynamicDescriptorHeap::CopyAndBindStagedTables(DescriptorHandleCache& HandleCache, ID3D12GraphicsCommandList* CmdList,
                void (STDMETHODCALLTYPE ID3D12GraphicsCommandList::* SetFunc)(UINT, D3D12_GPU_DESCRIPTOR_HANDLE))
            {
                uint32_t NeededSize = HandleCache.ComputeStagedSize();
                if (!HasSpace(NeededSize))
                {
                    RetireCurrentHeap();
                    UnbindAllValid();
                    NeededSize = HandleCache.ComputeStagedSize();
                }

                // This can trigger the creation of a new heap
                m_OwningContext.SetDescriptorHeap((EResourceHeapType)m_DescriptorType, this);
                HandleCache.CopyAndBindStaleTables(m_DescriptorType, m_DescriptorSize, Allocate(NeededSize), CmdList, SetFunc);
            }

            void D3D12DynamicDescriptorHeap::UnbindAllValid(void)
            {
                m_GraphicsHandleCache.UnbindAllValid();
                m_ComputeHandleCache.UnbindAllValid();
            }

            D3D12_GPU_DESCRIPTOR_HANDLE D3D12DynamicDescriptorHeap::UploadDirect(D3D12_CPU_DESCRIPTOR_HANDLE Handle)
            {
                ID3D12Device* pID3D12Device = RCast<ID3D12Device*>(g_pDevice->GetNativeDevice());
                IE_ASSERT(pID3D12Device != NULL);

                if (!HasSpace(1))
                {
                    RetireCurrentHeap();
                    UnbindAllValid();
                }

                m_OwningContext.SetDescriptorHeap((EResourceHeapType)m_DescriptorType, this);

                DescriptorHandle DestHandle = m_FirstDescriptor + m_CurrentOffset * m_DescriptorSize;
                m_CurrentOffset += 1;
                D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle{ DestHandle.GetCpuPtr() };
                D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle{ DestHandle.GetGpuPtr() };
                pID3D12Device->CopyDescriptorsSimple(1, CPUHandle, Handle, m_DescriptorType);

                return GPUHandle;
            }

            void D3D12DynamicDescriptorHeap::DescriptorHandleCache::UnbindAllValid()
            {
                m_StaleRootParamsBitMap = 0;

                unsigned long TableParams = m_RootDescriptorTablesBitMap;
                unsigned long RootIndex;
                while (_BitScanForward(&RootIndex, TableParams))
                {
                    TableParams ^= (1 << RootIndex);
                    if (m_RootDescriptorTable[RootIndex].AssignedHandlesBitMap != 0)
                        m_StaleRootParamsBitMap |= (1 << RootIndex);
                }
            }

            void D3D12DynamicDescriptorHeap::DescriptorHandleCache::StageDescriptorHandles(UInt32 RootIndex, UInt32 Offset, UInt32 NumHandles, const D3D12_CPU_DESCRIPTOR_HANDLE Handles[])
            {
                IE_ASSERT(((1 << RootIndex) & m_RootDescriptorTablesBitMap) != 0, "Root parameter is not a CBV_SRV_UAV descriptor table");
                IE_ASSERT(Offset + NumHandles <= m_RootDescriptorTable[RootIndex].TableSize);

                DescriptorTableCache& TableCache = m_RootDescriptorTable[RootIndex];
                D3D12_CPU_DESCRIPTOR_HANDLE* CopyDest = TableCache.TableStart + Offset;
                for (UInt32 i = 0; i < NumHandles; ++i)
                    CopyDest[i] = Handles[i];
                TableCache.AssignedHandlesBitMap |= ((1 << NumHandles) - 1) << Offset;
                m_StaleRootParamsBitMap |= (1 << RootIndex);
            }

            /*void D3D12DynamicDescriptorHeap::DescriptorHandleCache::ParseRootSignature(D3D12_DESCRIPTOR_HEAP_TYPE Type, const RootSignature& RootSig)
            {
                UInt32 CurrentOffset = 0;

                ASSERT(RootSig.m_NumParameters <= 16, "Maybe we need to support something greater");

                m_StaleRootParamsBitMap = 0;
                m_RootDescriptorTablesBitMap = (Type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER ?
                    RootSig.m_SamplerTableBitMap : RootSig.m_DescriptorTableBitMap);

                unsigned long TableParams = m_RootDescriptorTablesBitMap;
                unsigned long RootIndex;
                while (_BitScanForward(&RootIndex, TableParams))
                {
                    TableParams ^= (1 << RootIndex);

                    UInt32 TableSize = RootSig.m_DescriptorTableSize[RootIndex];
                    ASSERT(TableSize > 0);

                    DescriptorTableCache& RootDescriptorTable = m_RootDescriptorTable[RootIndex];
                    RootDescriptorTable.AssignedHandlesBitMap = 0;
                    RootDescriptorTable.TableStart = m_HandleCache + CurrentOffset;
                    RootDescriptorTable.TableSize = TableSize;

                    CurrentOffset += TableSize;
                }

                m_MaxCachedDescriptors = CurrentOffset;

                IE_ASSERT(m_MaxCachedDescriptors <= kMaxNumDescriptors, "Exceeded user-supplied maximum cache size");
            }*/

		}
	}
}
