#include <Engine_pch.h>

#include "Platform/DirectX12/Private/D3D12CommandManager.h"
#include "Platform/DirectX12/Public/D3D12Device.h"
#include "Platform/Win32/Error/COMException.h"
#include "D3D12CommandManager.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			void D3D12CommandManager::Initialize(IDevice* pDevice)
			{
				m_pDevice = DCast<D3D12Device*>(pDevice);
				IE_ASSERT(m_pDevice);

				//
				// Create the Command Queues
				//
				m_pGraphicsQueue = new D3D12CommandQueue(CQT_Direct);
				D3D12CommandQueue* pD3D12GraphicsCommandQueue = DCast<D3D12CommandQueue*>(m_pGraphicsQueue);
				IE_ASSERT(pD3D12GraphicsCommandQueue);
				pD3D12GraphicsCommandQueue->Initialize(m_pDevice);
			}

			void D3D12CommandManager::UnInitialize()
			{
			}




			void D3D12CommandQueue::Initialize(D3D12Device* pDevice)
			{
				IE_ASSERT(pDevice);
				ID3D12Device* pD3D12Device = RCast<ID3D12Device*>(pDevice->GetNativeDevice());
				IE_ASSERT(pD3D12Device);

				D3D12_COMMAND_QUEUE_DESC Desc = {};
				//ZeroMem(&Desc);
				Desc.Type = IECommandQueueTypeToD3DCommandQueueType(m_Type);
				Desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				HRESULT hr = pD3D12Device->CreateCommandQueue(&Desc, IID_PPV_ARGS(&m_pD3D12CommandQueue));
				ThrowIfFailed(hr, TEXT("Failed to create command queue!"));
			}
		}
	}
}
