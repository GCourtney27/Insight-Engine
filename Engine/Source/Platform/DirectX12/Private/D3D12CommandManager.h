#pragma once

#include "Runtime/Core.h"

#include "Runtime/Graphics/Private/ICommandManager.h"
#include "Platform/DirectX12/Private/D3D12CommandAllocatorPool.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class D3D12Device;
			class D3D12CommandManager;

			class INSIGHT_API D3D12CommandQueue final : public ICommandQueue
			{
				friend D3D12CommandManager;
			public:
				inline virtual void* GetNativeQueue() override { return RCast<void*>(m_pID3D12CommandQueue); }

				virtual void WaitforFence(UInt64 FenceValue) override;
				virtual bool IsFenceCompleted(UInt64 FenceValue) override;

				UInt64 ExecuteCommandList(ID3D12CommandList* pCommandList);
				
				inline void DiscardAllocator(UInt64 FenceValue, ID3D12CommandAllocator* pCommandAllocator)
				{
					m_D3D12AllocatorPool.DiscardAllocator(FenceValue, pCommandAllocator);
				}

				inline ID3D12CommandAllocator* RequestAllocator()
				{
					UInt64 CompletedValue = m_pFence->GetCompletedValue();
					return m_D3D12AllocatorPool.RequestAllocator(CompletedValue);
				}


			private:
				D3D12CommandQueue(const ECommandListType Type);
				virtual ~D3D12CommandQueue();

				void Initialize(ID3D12Device* pDevice);

				//
				// D3D Initializations
				//
				void CreateD3D12Queue();
				void CreateSyncObjects();
				

				ID3D12Device*				m_pID3DDeviceRef;
				
				ID3D12CommandQueue*			m_pID3D12CommandQueue;
				D3D12CommandAllocatorPool	m_D3D12AllocatorPool;

				// Sync
				//
				std::mutex		m_FenceMutex;
				std::mutex		m_EventMutex;
				ID3D12Fence*	m_pFence;
				UInt64			m_NextFenceValue;
				UInt64			m_LastCompletedFenceValue;
				HANDLE			m_FenceEventHandle;
			};


			class INSIGHT_API D3D12CommandManager final : public ICommandManager
			{
				friend class IRenderContextFactory;
				friend class D3D12RenderContextFactory;
			public:
				virtual void Initialize(IDevice* pDevice) override;
				virtual void CreateNewCommandContext(const ECommandListType& Type, ICommandContext** pContext, void** pCommandAllocator) override;

				virtual void WaitForFence(UInt64 FenceValue) override;

			protected:
				D3D12CommandManager()
					: m_pID3D12DeviceRef(NULL)
					, m_pD3D12DeviceRef(NULL)
					, m_D3D12GraphicsQueue(CLT_Direct)
					, m_D3D12ComputeQueue(CLT_Compute)
				{
				}
				virtual ~D3D12CommandManager() 
				{
					m_pID3D12DeviceRef = NULL;
					m_pD3D12DeviceRef = NULL;
				}

				virtual void UnInitialize() override;

				// References
				ID3D12Device* m_pID3D12DeviceRef;
				D3D12Device* m_pD3D12DeviceRef;


				D3D12CommandQueue m_D3D12GraphicsQueue;
				D3D12CommandQueue m_D3D12ComputeQueue;

			};
		}
	}
}

