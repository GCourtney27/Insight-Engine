#include <Engine_pch.h>

#include "Platform/DirectX12/Public/D3D12DescriptorHeap.h"

#include "Runtime/Graphics/Public/IDevice.h"


namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
            //
            // DescriptorHeap implementation
            //

            void D3D12DescriptorHeap::Create(const std::wstring& Name, EResourceHeapType Type, uint32_t MaxCount)
            {
                m_HeapDesc.Type = (D3D12_DESCRIPTOR_HEAP_TYPE)Type;
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

            DescriptorHandle D3D12DescriptorHeap::Alloc(uint32_t Count)
            {
                IE_ASSERT(HasAvailableSpace(Count), "Descriptor Heap out of space.  Increase heap size.");
                DescriptorHandle ret = m_NextFreeHandle;
                m_NextFreeHandle += Count * m_DescriptorSize;
                m_NumFreeDescriptors -= Count;
                return ret;
            }

            bool D3D12DescriptorHeap::ValidateHandle(const DescriptorHandle& DHandle) const
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
