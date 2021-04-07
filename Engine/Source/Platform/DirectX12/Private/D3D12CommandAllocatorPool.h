#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/CommonEnums.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class INSIGHT_API D3D12CommandAllocatorPool
			{
			public:
				D3D12CommandAllocatorPool(const D3D12_COMMAND_LIST_TYPE& Type);
				~D3D12CommandAllocatorPool();

				void Initialize(ID3D12Device* pD3DDevice);

				ID3D12CommandAllocator* RequestAllocator(UInt64 CompletedFenceValue);
				void DiscardAllocator(UInt64 FenceValue, ID3D12CommandAllocator* pAllocator);

				inline UInt64 Size() const { return m_AllocatorPool.size(); }

			protected:
				const D3D12_COMMAND_LIST_TYPE m_cCommandListType;
				ID3D12Device* m_pID3D12Device;
				std::vector<ID3D12CommandAllocator*> m_AllocatorPool;
				std::queue<std::pair<UInt64, ID3D12CommandAllocator*>> m_ReadyAllocators;
				std::mutex m_AllocatorMutex;
			};
		}
	}
}
