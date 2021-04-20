#include <Engine_pch.h>

#include "Platform/DirectX12/Private/D3D12CommandManager.h"
#include "Platform/DirectX12/Public/D3D12CommandContext.h"
#include "Platform/DirectX12/Public/D3D12Device.h"
#include "Platform/Public/Utility/COMException.h"
#include "Platform/Public/Utility/APIBridge/D3DUtility.h"

#include "Runtime/Graphics/Public/GraphicsCore.h"


namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			// ---------------------
			// D3D12 Command Manager
			// ---------------------

			void D3D12CommandManager::Initialize(IDevice* pDevice)
			{
				m_pDeviceRef = DCast<D3D12Device*>(pDevice);
				IE_ASSERT(m_pDeviceRef);

				m_pID3D12DeviceRef = RCast<ID3D12Device*>(m_pDeviceRef->GetNativeDevice());

				//
				// Create the Command Queues
				//
				m_pGraphicsQueue = &m_D3D12GraphicsQueue;
				m_D3D12GraphicsQueue.Initialize(m_pID3D12DeviceRef);
				// TODO: Create command queue
			}

			void D3D12CommandManager::UnInitialize()
			{
				m_pD3D12DeviceRef = NULL;
				m_pID3D12DeviceRef = NULL;
			}

			void D3D12CommandManager::CreateNewCommandContext(const ECommandListType& Type, ICommandContext** pContext, void** pCommandAllocator)
			{
				switch (Type)
				{
				case ECommandListType::CLT_Direct:
					(*pCommandAllocator) = m_D3D12GraphicsQueue.RequestAllocator();
					break;
				case ECommandListType::CLT_Compute:
					break;
				default:
					break;
				}

				ID3D12GraphicsCommandList** pD3D12CmdList = RCast<ID3D12GraphicsCommandList**>( (*pContext)->GetNativeContextAddress() );

				ThrowIfFailed(
					m_pID3D12DeviceRef->CreateCommandList(0, PlatformUtils::IECommandListTypeToD3DCommandListType(Type), RCast<ID3D12CommandAllocator*>(*pCommandAllocator), NULL, IID_PPV_ARGS( pD3D12CmdList))
					, TEXT("Failed to create command list!"));
			}

			void D3D12CommandManager::WaitForFence(UInt64 FenceValue)
			{
				ICommandQueue* Queue = g_pCommandManager->GetQueue( (ECommandListType)(FenceValue >> 56) );
				Queue->WaitForFence(FenceValue);
			}



			// -----------------------
			//  D3D12 Command Queue
			// -----------------------

			D3D12CommandQueue::D3D12CommandQueue(const ECommandListType Type)
				: ICommandQueue(Type)
				, m_pID3D12CommandQueue(NULL)
				, m_pID3DDeviceRef(NULL)
				, m_D3D12AllocatorPool(PlatformUtils::IECommandListTypeToD3DCommandListType(Type))
				, m_pFence(NULL)
				, m_NextFenceValue(0u)
				, m_LastCompletedFenceValue(0u)
				, m_FenceEventHandle(NULL)
			{
			}
			
			UInt64 D3D12CommandQueue::ExecuteCommandList(ID3D12CommandList* pCommandList)
			{
				std::lock_guard<std::mutex> LockGuard(m_FenceMutex);

				ThrowIfFailed( ((ID3D12GraphicsCommandList*)pCommandList)->Close(), TEXT("Failed to close command list!") );

				m_pID3D12CommandQueue->ExecuteCommandLists(1, &pCommandList);
				
				m_pID3D12CommandQueue->Signal(m_pFence, m_NextFenceValue);

				return m_NextFenceValue++;
			}

			void D3D12CommandQueue::WaitForFence(UInt64 FenceValue)
			{
				if (IsFenceCompleted(FenceValue))
					return;

				{
					std::lock_guard<std::mutex> LockGaurd(m_EventMutex);

					m_pFence->SetEventOnCompletion(FenceValue, m_FenceEventHandle);
					WaitForSingleObject(m_FenceEventHandle, INFINITE);
					m_LastCompletedFenceValue = FenceValue;
				}
			}
			
			UInt64 D3D12CommandQueue::IncrementFence()
			{
				std::lock_guard<std::mutex> LockGuard(m_FenceMutex);
				m_pID3D12CommandQueue->Signal(m_pFence, m_NextFenceValue);
				return m_NextFenceValue++;
			}

			bool D3D12CommandQueue::IsFenceCompleted(UInt64 FenceValue)
			{
				if (FenceValue > m_LastCompletedFenceValue)
					m_LastCompletedFenceValue = std::max(m_LastCompletedFenceValue, m_pFence->GetCompletedValue());
					
				return FenceValue <= m_LastCompletedFenceValue;
			}

			D3D12CommandQueue::~D3D12CommandQueue()
			{
				CloseHandle(m_FenceEventHandle);

				COM_SAFE_RELEASE(m_pFence);
				COM_SAFE_RELEASE(m_pID3D12CommandQueue);
			}

			void D3D12CommandQueue::Initialize(ID3D12Device* pID3D12Device)
			{
				IE_ASSERT(pID3D12Device);
				m_pID3DDeviceRef = pID3D12Device;

				m_D3D12AllocatorPool.Initialize(pID3D12Device);
				CreateD3D12Queue();
				CreateSyncObjects();
			}
			
			void D3D12CommandQueue::CreateD3D12Queue()
			{
				D3D12_COMMAND_QUEUE_DESC Desc;
				ZeroMem(&Desc);
				Desc.Type = PlatformUtils::IECommandListTypeToD3DCommandListType(m_Type);
				Desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				HRESULT hr = m_pID3DDeviceRef->CreateCommandQueue(&Desc, IID_PPV_ARGS(&m_pID3D12CommandQueue));
				ThrowIfFailed(hr, TEXT("Failed to create command queue!"));
			}
			
			void D3D12CommandQueue::CreateSyncObjects()
			{
				HRESULT hr = S_OK;
				
				hr = m_pID3DDeviceRef->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
				ThrowIfFailed(hr, TEXT("Failed to create fence object!"));
				m_pFence->SetName(L"CommandListManager::m_pFence");
				
				D3D12_COMMAND_LIST_TYPE Type = PlatformUtils::IECommandListTypeToD3DCommandListType(m_Type);
				hr = m_pFence->Signal(SCast<UInt64>(Type) << 56);
				ThrowIfFailed(hr, TEXT("Failed to signal fence object!"));

				m_FenceEventHandle = CreateEvent(nullptr, false, false, nullptr);
				IE_ASSERT(m_FenceEventHandle != NULL);
			}
		}
	}
}
