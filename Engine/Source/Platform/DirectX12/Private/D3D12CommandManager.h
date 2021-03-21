#pragma once

#include "Runtime/Core.h"

#include "Runtime/Graphics/Private/ICommandManager.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class D3D12Device;

			class INSIGHT_API D3D12CommandQueue final : public ICommandQueue
			{
				friend class D3D12CommandManager;
			public:
				inline virtual void* GetNativeQueue() override { return RCast<void*>(m_pD3D12CommandQueue); }

			protected:
				void Initialize(D3D12Device* pDevice);

				D3D12CommandQueue(const ECommandQueueType& Type)
					: ICommandQueue(Type)
					, m_pD3D12CommandQueue(NULL)
				{
				}
				virtual ~D3D12CommandQueue()
				{
					COM_SAFE_RELEASE(m_pD3D12CommandQueue);
				}

				ID3D12CommandQueue* m_pD3D12CommandQueue;
			};


			class INSIGHT_API D3D12CommandManager final : public ICommandManager
			{
				friend class D3D12RenderContextFactory;
			public:
				virtual void Initialize(IDevice* pDevice) override;

			protected:
				D3D12CommandManager() {}
				~D3D12CommandManager() {}

				virtual void UnInitialize() override;

				D3D12Device* m_pDevice;

			};
		}
	}
}

