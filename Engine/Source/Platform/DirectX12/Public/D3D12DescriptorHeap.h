#pragma once

#include "EngineDefines.h"
#include <Runtime/Graphics/Public/GraphicsCore.h>

#include "Graphics/Public/IDescriptorHeap.h"
#include "Platform/DirectX12/Public/Common/D3D12Utility.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class INSIGHT_API D3D12DescriptorHeap : public IDescriptorHeap
			{
            public:
                D3D12DescriptorHeap(void) {}
                ~D3D12DescriptorHeap(void) { Destroy(); }

                virtual void* GetNativeHeap() override { return RCast<ID3D12DescriptorHeap*>(m_Heap.Get()); }


                virtual void Create(const FString& DebugHeapName, EResourceHeapType Type, uint32_t MaxCount) override;
                void Destroy(void) { m_Heap = nullptr; }

                bool HasAvailableSpace(uint32_t Count) const { return Count <= m_NumFreeDescriptors; }
                virtual DescriptorHandle Alloc(UInt32 Count = 1) override;

                DescriptorHandle operator[] (uint32_t arrayIdx) const { return m_FirstHandle + arrayIdx * m_DescriptorSize; }

                uint32_t GetOffsetOfHandle(const DescriptorHandle& DHandle) {
                    return (uint32_t)(DHandle.GetCpuPtr() - m_FirstHandle.GetCpuPtr()) / m_DescriptorSize;
                }

                bool ValidateHandle(const DescriptorHandle& DHandle) const;

                ID3D12DescriptorHeap* GetHeapPointer() const { return m_Heap.Get(); }

                uint32_t GetDescriptorSize(void) const { return m_DescriptorSize; }

            private:

                Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
                D3D12_DESCRIPTOR_HEAP_DESC m_HeapDesc;
                uint32_t m_DescriptorSize;
                uint32_t m_NumFreeDescriptors;
                DescriptorHandle m_FirstHandle;
                DescriptorHandle m_NextFreeHandle;
			};
		}
	}
}
