#pragma once
#include "Runtime/Graphics/Public/CommonEnums.h"
#include "Runtime/Graphics/Public/CommonStructs.h"

#include "Platform/DirectX12/Public/D3D12DescriptorAllocator.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			extern class D3D12DescriptorAllocator g_DescriptorAllocator[];
			inline D3D12_CPU_DESCRIPTOR_HANDLE AllocateDescriptor(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE Type, UInt32 Count = 1)
			{
				return g_DescriptorAllocator[Type].Allocate(pDevice, Count);
			}
		}
	}
}
