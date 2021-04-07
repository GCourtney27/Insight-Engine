#include <Engine_pch.h>

#include "Platform/DirectX12/Private/D3D12CommonGlobals.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			D3D12DescriptorAllocator g_DescriptorAllocator[RHT_HeapType_Count]
			{
				RHT_CBV_SRV_UAV,
				RHT_Sampler,
				RHT_RTV,
				RHT_DSV,
			};

		}
	}
}
