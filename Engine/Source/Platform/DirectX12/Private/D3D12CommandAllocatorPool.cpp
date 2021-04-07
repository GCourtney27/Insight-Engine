#include <Engine_pch.h>

#include "Platform/DirectX12/Private/D3D12CommandAllocatorPool.h"
#include "Platform/Public/Utility/COMException.h"
#include "Platform/Public/Utility/APIBridge/D3DUtility.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			D3D12CommandAllocatorPool::D3D12CommandAllocatorPool(const D3D12_COMMAND_LIST_TYPE& Type)
				: m_cCommandListType(Type)
				, m_pID3D12Device(NULL)
			{
			}

			D3D12CommandAllocatorPool::~D3D12CommandAllocatorPool()
			{
				m_pID3D12Device = NULL;
			}

			void D3D12CommandAllocatorPool::Initialize(ID3D12Device* pD3DDevice)
			{
				IE_ASSERT(pD3DDevice != NULL);
				m_pID3D12Device = pD3DDevice;
			}

			ID3D12CommandAllocator* D3D12CommandAllocatorPool::RequestAllocator(UInt64 CompletedFenceValue)
			{
				std::lock_guard<std::mutex> LockGaurd(m_AllocatorMutex);

				ID3D12CommandAllocator* pAllocator = NULL;

				if (!m_ReadyAllocators.empty())
				{
					std::pair<UInt64, ID3D12CommandAllocator*>& AllocatorPair = m_ReadyAllocators.front();

					if (AllocatorPair.first <= CompletedFenceValue)
					{
						pAllocator = AllocatorPair.second;
						ThrowIfFailed(pAllocator->Reset(), TEXT("Failed to reset command allocator!"));
						m_ReadyAllocators.pop();
					}
				}
				
				// If no allocator's were ready to be reused, create a new one
				if (pAllocator == nullptr)
				{
					ThrowIfFailed(m_pID3D12Device->CreateCommandAllocator(m_cCommandListType, IID_PPV_ARGS(&pAllocator))
						, TEXT("Failed to create command allocator"));
					wchar_t AllocatorName[32];
					swprintf(AllocatorName, 32, L"CommandAllocator %zu", m_AllocatorPool.size());
					pAllocator->SetName(AllocatorName);
					m_AllocatorPool.push_back(pAllocator);
				}

				return pAllocator;
			}

			void D3D12CommandAllocatorPool::DiscardAllocator(UInt64 FenceValue, ID3D12CommandAllocator* pAllocator)
			{
				std::lock_guard<std::mutex> LockGuard(m_AllocatorMutex);

				// That fence value indicates we are free to reset the allocator
				m_ReadyAllocators.push(std::make_pair(FenceValue, pAllocator));
			}
		}
	}
}
