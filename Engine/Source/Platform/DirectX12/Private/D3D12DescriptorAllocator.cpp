#include <Engine_pch.h>

#include "Platform/DirectX12/Public/D3D12DescriptorAllocator.h"

#include "Platform/Public/Utility/COMException.h"
#include "Runtime/Graphics/Public/IDevice.h"

namespace Insight
{
	namespace Graphics
	{
        namespace DX12
        {
            std::mutex D3D12DescriptorAllocator::sm_AllocationMutex;
            std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> D3D12DescriptorAllocator::sm_DescriptorHeapPool;

            void D3D12DescriptorAllocator::DestroyAll(void)
            {
                sm_DescriptorHeapPool.clear();
            }

            ID3D12DescriptorHeap* D3D12DescriptorAllocator::RequestNewHeap(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE Type)
            {
                std::lock_guard<std::mutex> LockGuard(sm_AllocationMutex);
                IE_ASSERT(pDevice != NULL)

                D3D12_DESCRIPTOR_HEAP_DESC Desc;
                Desc.Type = Type;
                Desc.NumDescriptors = sm_NumDescriptorsPerHeap;
                Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                Desc.NodeMask = 1;

                Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pHeap;
                HRESULT hr = pDevice->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&pHeap));
                ThrowIfFailed(hr, TEXT("Failed ot create descriptor heap!"));
                sm_DescriptorHeapPool.emplace_back(pHeap);
                return pHeap.Get();
            }

            D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorAllocator::Allocate(ID3D12Device* pDevice, UInt32 Count)
            {
                IE_ASSERT(pDevice != NULL);

                if (m_CurrentHeap == NULL || m_RemainingFreeHandles < Count)
                {
                    m_CurrentHeap = RequestNewHeap(pDevice, m_Type);
                    m_CurrentHandle = m_CurrentHeap->GetCPUDescriptorHandleForHeapStart();
                    m_RemainingFreeHandles = sm_NumDescriptorsPerHeap;

                    if (m_DescriptorSize == 0)
                        m_DescriptorSize = pDevice->GetDescriptorHandleIncrementSize(m_Type);
                }

                D3D12_CPU_DESCRIPTOR_HANDLE ret = m_CurrentHandle;
                m_CurrentHandle.ptr += Count * m_DescriptorSize;
                m_RemainingFreeHandles -= Count;
                return ret;
            }


            //
            // DescriptorHeap implementation
            //

            void DescriptorHeap::Create(const std::wstring& Name, D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t MaxCount)
            {
                m_HeapDesc.Type = Type;
                m_HeapDesc.NumDescriptors = MaxCount;
                m_HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                m_HeapDesc.NodeMask = 1;

                ID3D12Device* pID3D12Device = RCast<ID3D12Device*>(g_pDevice->GetNativeDevice());
                IE_ASSERT(pID3D12Device != NULL);

                pID3D12Device->CreateDescriptorHeap(&m_HeapDesc, IID_PPV_ARGS(m_Heap.ReleaseAndGetAddressOf()));

#if IE_DEBUG
                (void)Name;
#else
                m_Heap->SetName(Name.c_str());
#endif

                m_DescriptorSize = pID3D12Device->GetDescriptorHandleIncrementSize(m_HeapDesc.Type);
                m_NumFreeDescriptors = m_HeapDesc.NumDescriptors;
                m_FirstHandle = DescriptorHandle(
                    m_Heap->GetCPUDescriptorHandleForHeapStart(),
                    m_Heap->GetGPUDescriptorHandleForHeapStart());
                m_NextFreeHandle = m_FirstHandle;
            }

            DescriptorHandle DescriptorHeap::Alloc(uint32_t Count)
            {
                IE_ASSERT(HasAvailableSpace(Count), "Descriptor Heap out of space.  Increase heap size.");
                DescriptorHandle ret = m_NextFreeHandle;
                m_NextFreeHandle += Count * m_DescriptorSize;
                m_NumFreeDescriptors -= Count;
                return ret;
            }

            bool DescriptorHeap::ValidateHandle(const DescriptorHandle& DHandle) const
            {
                if (DHandle.GetCpuPtr() < m_FirstHandle.GetCpuPtr() ||
                    DHandle.GetCpuPtr() >= m_FirstHandle.GetCpuPtr() + m_HeapDesc.NumDescriptors * m_DescriptorSize)
                    return false;

                if (DHandle.GetGpuPtr() - m_FirstHandle.GetGpuPtr() !=
                    DHandle.GetCpuPtr() - m_FirstHandle.GetCpuPtr())
                    return false;

                return true;
            }
        }
	}
}
