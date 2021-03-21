#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Private/ISwapChain.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class D3D12CommandQueue;

			class INSIGHT_API D3D12SwapChain final : public ISwapChain
			{
			public:
				D3D12SwapChain();
				~D3D12SwapChain();
				
				virtual void SwapBuffers() override;
				virtual void Resize() override;

				virtual void* GetNativeSwapChain() const { return SCast<void*>(m_pDXGISwapChain); }

				virtual void ToggleFullScreen(bool IsEnabled) override;

				void Initialize(const IESwapChainCreateDesc& InitParams, IDXGIFactory6** ppDXGIFactory, D3D12CommandQueue* ppCommandQueue);

			protected:
				virtual void UnInitialize() override;

			protected:
				IDXGISwapChain3* m_pDXGISwapChain;
				DXGI_SWAP_CHAIN_DESC1 m_Desc;
			};
		}
	}
}