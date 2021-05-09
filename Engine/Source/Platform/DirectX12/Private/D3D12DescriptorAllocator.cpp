#include <Engine_pch.h>

#include "Platform/DirectX12/Public/D3D12DescriptorAllocator.h"

#include "Platform/Public/Utility/COMException.h"

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

        }
	}
}
