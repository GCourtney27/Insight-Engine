#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/ISwapChain.h"

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

				virtual void Initialize(IDevice* pDevice) override;

				virtual void SwapBuffers() override;
				virtual void Resize(const FVector2& NewResolution) override;
				virtual void SetNumBackBuffes(UInt32 NumBuffers) override;
				virtual void SetBackBufferFormat(EFormat& Format) override;

				virtual void* GetNativeSwapChain() const { return SCast<void*>(m_pDXGISwapChain); }

				virtual void ToggleFullScreen(bool IsEnabled) override;

				void Create(const IESwapChainDescription& InitParams, IDXGIFactory6** ppDXGIFactory, D3D12CommandQueue* ppCommandQueue, ID3D12Device* pDevice);


			protected:
				virtual void UnInitialize() override;
				//
				// Utility
				//
				void ResizeDXGIBuffers();


				//
				// D3D Initialize
				//
				void BindSwapChainBackBuffers();
				void CheckTearingSupport(IDXGIFactory6* pFactory);

			protected:

				ID3D12Device* m_pID3D12DeviceRef;

				IDXGISwapChain3* m_pDXGISwapChain;

			};
		}
	}
}