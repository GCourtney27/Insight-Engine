#pragma once

#include <Runtime/Core.h>
#include <Runtime/Graphics/Public/GraphicsCore.h>


namespace Insight
{
	namespace Graphics
	{
        // This handle refers to a descriptor or a descriptor table (contiguous descriptors) that is shader visible.
        class DescriptorHandle
        {
        public:
            DescriptorHandle()
            {
                m_CpuHandle.Ptr = IE_INVALID_GPU_ADDRESS;
                m_GpuHandle.Ptr = IE_INVALID_GPU_ADDRESS;
            }

            /*
            // Should we allow constructing handles that might not be shader visible?
            DescriptorHandle( D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle )
                : m_CpuHandle(CpuHandle)
            {
                m_GpuHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
            }
            */

            DescriptorHandle(CpuDescriptorHandle CpuHandle, GpuDescriptorHandle GpuHandle)
                : m_CpuHandle(CpuHandle), m_GpuHandle(GpuHandle)
            {
            }

            DescriptorHandle operator+ (INT OffsetScaledByDescriptorSize) const
            {
                DescriptorHandle ret = *this;
                ret += OffsetScaledByDescriptorSize;
                return ret;
            }

            void operator += (INT OffsetScaledByDescriptorSize)
            {
                if (m_CpuHandle.Ptr != IE_INVALID_GPU_ADDRESS)
                    m_CpuHandle.Ptr += OffsetScaledByDescriptorSize;
                if (m_GpuHandle.Ptr != IE_INVALID_GPU_ADDRESS)
                    m_GpuHandle.Ptr += OffsetScaledByDescriptorSize;
            }

            const CpuDescriptorHandle* operator&() const { return &m_CpuHandle; }
            operator CpuDescriptorHandle() const { return m_CpuHandle; }
            operator GpuDescriptorHandle() const { return m_GpuHandle; }

            size_t GetCpuPtr() const { return m_CpuHandle.Ptr; }
            uint64_t GetGpuPtr() const { return m_GpuHandle.Ptr; }
            bool IsNull() const { return m_CpuHandle.Ptr == IE_INVALID_GPU_ADDRESS; }
            bool IsShaderVisible() const { return m_GpuHandle.Ptr != IE_INVALID_GPU_ADDRESS; }

        private:
            CpuDescriptorHandle m_CpuHandle;
            GpuDescriptorHandle m_GpuHandle;
        };
		class INSIGHT_API IDescriptorHeap
		{
		public:
			virtual void* GetNativeHeap() = 0;

			virtual DescriptorHandle Alloc(UInt32 Count) = 0;

		protected:
			IDescriptorHeap() {}
			virtual ~IDescriptorHeap() {}

			virtual void Create(const FString& DebugHeapName, EResourceHeapType Type, uint32_t MaxCount) = 0;

		};
	}
}
